#include "terrain_mesh_builder.h"
#include "outline_ids.h"
#include "voxel_tree_builder.h"
#include "terrain_editor_state.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace game
{
    namespace
    {
        using point = std::array<float, 3>;
        using color = std::array<uint8_t, 4>;
        color blend_color(color a,color b,float t){
            color result={{0,0,0,255}};
            for(int i=0;i<3;++i)result[i]=static_cast<uint8_t>(std::round(a[i]+(b[i]-a[i])*t));
            return result;
        }
        color terrace_color(int label,int levels){
            // Linear vertex colors, coast first: sand, grass, soil, then rocks.
            const std::array<color,8> palette={{{{194,141,64,255}},{{113,132,31,255}},
                {{74,122,24,255}},{{41,108,21,255}},{{55,25,10,255}},
                {{55,25,10,255}},{{100,106,108,255}},{{128,134,138,255}}}};
            const float index=std::max(0.f,std::min(7.f,label*7.f/std::max(levels-1,1)));
            const int lower=static_cast<int>(index),upper=std::min(lower+1,7);
            return blend_color(palette[lower],palette[upper],index-lower);
        }
        color terrace_inner_color(int label,int levels){
            const float index=label*7.f/std::max(levels-1,1);
            if(std::abs(index-3.f)<.01f)return {{55,25,10,255}};
            if(std::abs(index-5.f)<.01f)return {{77,83,83,255}};
            return terrace_color(label,levels);
        }

        // Append independent vertices so tops never share normals with walls.
        void triangle(terrain_mesh_data& mesh, point a, point b, point c, point normal, color tint,uint32_t outline_id=0)
        {
            if (mesh.vertices.size() + 3 > 65535)
                throw std::invalid_argument("Island mesh exceeds 16-bit vertex capacity");
            for (const auto& p : {a, b, c})
            {
                mesh.indices.push_back(static_cast<uint16_t>(mesh.vertices.size()));
                mesh.vertices.push_back({p, normal, tint});
                mesh.vertices.back().outline_id=outline_id;
            }
        }

        float occupancy(const terrain_data& data, float x, float z, int layer)
        {
            const float cell = data.config.world_size / data.config.resolution;
            const float gx = (x + data.config.world_size*.5f) / cell - .5f;
            const float gz = (z + data.config.world_size*.5f) / cell - .5f;
            const int ix = static_cast<int>(std::floor(gx)), iz = static_cast<int>(std::floor(gz));
            const float tx = gx-ix, tz = gz-iz;
            const float a = data.layer_at(ix, iz) >= layer ? 1.f : 0.f;
            const float b = data.layer_at(ix+1, iz) >= layer ? 1.f : 0.f;
            const float c = data.layer_at(ix, iz+1) >= layer ? 1.f : 0.f;
            const float d = data.layer_at(ix+1, iz+1) >= layer ? 1.f : 0.f;
            return (a*(1.f-tx)+b*tx)*(1.f-tz)+(c*(1.f-tx)+d*tx)*tz;
        }

        point subtract(point a,point b){return {{a[0]-b[0],a[1]-b[1],a[2]-b[2]}};}
        point add(point a,point b){return {{a[0]+b[0],a[1]+b[1],a[2]+b[2]}};}
        point scale(point p,float s){return {{p[0]*s,p[1]*s,p[2]*s}};}
        point cross(point a,point b){return {{a[1]*b[2]-a[2]*b[1],a[2]*b[0]-a[0]*b[2],a[0]*b[1]-a[1]*b[0]}};}
        point normalized(point p){return scale(p,1.f/std::sqrt(p[0]*p[0]+p[1]*p[1]+p[2]*p[2]));}
        color shaded(color c,float s){for(int i=0;i<3;++i)c[i]=static_cast<uint8_t>(std::min(255.f,c[i]*s));return c;}
        void tree_face(terrain_mesh_data& mesh,point a,point b,point c,color tint){triangle(mesh,a,b,c,normalized(cross(subtract(b,a),subtract(c,a))),tint);}
        void tree_branch(terrain_mesh_data& mesh,point a,point b,float lower,float upper,int sides,color tint){
            const auto axis=normalized(subtract(b,a));
            const auto u=normalized(cross(axis,std::abs(axis[1])>.95f?point{{1,0,0}}:point{{0,1,0}}));
            const auto v=cross(axis,u);
            for(int i=0;i<sides;++i){
                const float angle=i*6.283185307f/sides,next=(i+1)*6.283185307f/sides;
                const auto p=add(scale(u,std::cos(angle)),scale(v,std::sin(angle)));
                const auto q=add(scale(u,std::cos(next)),scale(v,std::sin(next)));
                const auto x=add(a,scale(p,lower)),y=add(a,scale(q,lower));
                const auto z=add(b,scale(p,upper)),w=add(b,scale(q,upper));
                const auto face=shaded(tint,.9f+.12f*std::cos(angle));
                tree_face(mesh,x,y,z,face);
                if(upper>0.f){tree_face(mesh,y,w,z,face);tree_face(mesh,b,z,w,shaded(tint,1.05f));}
                tree_face(mesh,a,y,x,shaded(tint,.75f));
            }
        }
        template<class Random> void tree_crown(terrain_mesh_data& mesh,point center,point radius,color tint,Random& random){
            const float t=1.618033989f;
            point vertices[]={{{-1,t,0}},{{1,t,0}},{{-1,-t,0}},{{1,-t,0}},{{0,-1,t}},{{0,1,t}},
                {{0,-1,-t}},{{0,1,-t}},{{t,0,-1}},{{t,0,1}},{{-t,0,-1}},{{-t,0,1}}};
            for(auto& p:vertices){p=normalized(p);const float jitter=.9f+random()*.2f;for(int i=0;i<3;++i)p[i]=center[i]+p[i]*radius[i]*jitter;}
            const int faces[][3]={{0,11,5},{0,5,1},{0,1,7},{0,7,10},{0,10,11},{1,5,9},{5,11,4},{11,10,2},{10,7,6},{7,1,8},
                {3,9,4},{3,4,2},{3,2,6},{3,6,8},{3,8,9},{4,9,5},{2,4,11},{6,2,10},{8,6,7},{9,8,1}};
            for(const auto& f:faces)tree_face(mesh,vertices[f[0]],vertices[f[1]],vertices[f[2]],shaded(tint,.88f+random()*.24f));
        }

        void validate_layer_mask(const terrain_data& data, int layer)
        {
            const int size = data.config.resolution;
            // Flood both land and ocean. This checks every cell, including cells
            // between radial rays, and prevents silently dropping islands/holes.
            for (bool land : {true, false})
            {
                std::vector<uint8_t> visited(data.layers.size(), 0);
                std::vector<size_t> queue;
                queue.reserve(data.layers.size());
                const auto enqueue = [&](int x, int z) {
                    if (x < 0 || z < 0 || x >= size || z >= size) return;
                    const size_t index = static_cast<size_t>(z) * size + x;
                    if (!visited[index] && ((data.layers[index] >= layer) == land))
                    {
                        visited[index] = 1;
                        queue.push_back(index);
                    }
                };
                if (land) enqueue(size / 2, size / 2);
                else for (int i = 0; i < size; ++i)
                {
                    enqueue(i, 0); enqueue(i, size - 1);
                    enqueue(0, i); enqueue(size - 1, i);
                }
                for (size_t head = 0; head < queue.size(); ++head)
                {
                    const int x = static_cast<int>(queue[head] % size);
                    const int z = static_cast<int>(queue[head] / size);
                    enqueue(x - 1, z); enqueue(x + 1, z);
                    enqueue(x, z - 1); enqueue(x, z + 1);
                }
                for (size_t i = 0; i < data.layers.size(); ++i)
                    if (((data.layers[i] >= layer) == land) && !visited[i])
                        throw std::invalid_argument("Terrain layer has disconnected land or a hole");
            }
        }
    }

    terrain_mesh_data terrain_mesh_builder::build(const terrain_data& data)
    {
        const auto& config = data.config;
        config.validate();
        if (data.layers.size() != static_cast<size_t>(config.resolution)*config.resolution)
            throw std::invalid_argument("Terrain cell count does not match resolution");
        for (int layer : data.layers)
            if (layer != terrain_data::empty_layer && (layer < -config.underwater_layers_count || layer >= config.layers_count))
                throw std::invalid_argument("Terrain cell layer out of range");
        const int count = config.layers_count + config.underwater_layers_count;
        for (int layer = -config.underwater_layers_count; layer < config.layers_count; ++layer)
            validate_layer_mask(data, layer);
        const int segments = std::min(config.resolution, 320);
        const float cell = config.world_size/config.resolution;
        std::vector<std::vector<float>> radii(count, std::vector<float>(segments));
        for (int layer = 0; layer < count; ++layer)
        for (int i = 0; i < segments; ++i)
        {
            const float angle = i*6.283185307f/segments;
            const float dx = std::cos(angle), dz = std::sin(angle);
            const int label = layer - config.underwater_layers_count;

            if (occupancy(data, 0.f, 0.f, label) < .5f)
                throw std::invalid_argument("Terrain layer must contain the origin");
            float boundary = 0.f;
            bool outside = false;
            // Scan, rather than binary-search: reject a second occupied interval.
            for (float r = cell*.25f; r <= config.world_size*.72f; r += cell*.25f)
            {
                const bool inside = occupancy(data, dx*r, dz*r, label) >= .5f;
                if (outside && inside)
                    throw std::invalid_argument("Terrain layer is not star-shaped");
                if (!inside && !outside)
                {
                    float lo = r-cell*.25f, hi = r;
                    for (int step = 0; step < 12; ++step)
                    {
                        const float mid = (lo+hi)*.5f;
                        if (occupancy(data, dx*mid, dz*mid, label) >= .5f) lo=mid;
                        else hi=mid;
                    }
                    boundary=(lo+hi)*.5f;
                    outside=true;
                }
            }
            if (!outside || boundary < cell)
                throw std::invalid_argument("Terrain contour is missing or too small");
            radii[layer][i] = boundary;
        }
        // Positive convolution preserves nesting while removing grid stair steps.
        for (auto& ring : radii)
        for (int pass=0; pass<4; ++pass)
        {
            const auto previous = ring;
            for (int i=0; i<segments; ++i)
                ring[i] = (previous[(i+segments-2)%segments] + 4.f*previous[(i+segments-1)%segments]
                         + 6.f*previous[i] + 4.f*previous[(i+1)%segments] + previous[(i+2)%segments])/16.f;
        }
        terrain_mesh_data mesh;
        // Four triangles per contour segment/layer, except the innermost top fan has one.
        const size_t vertex_count = static_cast<size_t>(4*count-1)*segments*3;
        mesh.vertices.reserve(std::min<size_t>(vertex_count,65535));
        mesh.indices.reserve(std::min<size_t>(vertex_count,65535));
        const point up = {{0.f, 1.f, 0.f}};
        const auto at = [&](int layer, int i, float height) -> point {
            const float angle=i*6.283185307f/segments;
            return {{radii[layer][i]*std::cos(angle), height, radii[layer][i]*std::sin(angle)}};
        };
        for (int layer=0; layer<count; ++layer)
        {
            const int label = layer - config.underwater_layers_count;
            const auto silhouette=outline_terrace_id(label>=0?label+1:label);
            const float y=label==0 ? config.shore_height : label*config.layer_height;
            const color land = terrace_color(label,config.layers_count);
            const color debug = {{static_cast<uint8_t>(35+layer*13), static_cast<uint8_t>(210-layer*9), static_cast<uint8_t>(60+(layer%4)*45), 255}};
            const color soil = {{static_cast<uint8_t>(120+layer*3), static_cast<uint8_t>(85+layer*3),
                                 static_cast<uint8_t>(54+layer*2), 255}};
            for (int i=0; i<segments; ++i)
            {
                const int j=(i+1)%segments;
                const auto a=at(layer,i,y), b=at(layer,j,y);
                const color tint = config.debug_layer_colors ? debug : (label < 0 ? color{{177,163,118,255}} : land);
                if (layer+1 < count)
                {
                    if (radii[layer][i] <= radii[layer+1][i] || radii[layer][j] <= radii[layer+1][j])
                        throw std::invalid_argument("Terrain terraces must have positive width");
                    const auto c=at(layer+1,i,y), d=at(layer+1,j,y);
                    triangle(mesh,a,c,b,up,tint,silhouette);
                    triangle(mesh,b,c,d,up,tint,silhouette);
                    if(label>=0&&!config.debug_layer_colors){
                        const color inner=terrace_inner_color(label,config.layers_count);
                        // Inner edge of each annulus meets the next terrace's wall.
                        const size_t start=mesh.vertices.size()-6;
                        for(size_t v:{start+1,start+4,start+5})mesh.vertices[v].color=inner;
                    }
                }
                else triangle(mesh,a,{{0.f,y,0.f}},b,up,tint,silhouette);
                const float bottom=layer==0 ? std::min(config.water_y-config.layer_height, y-config.layer_height)
                    : (label==0 ? -config.layer_height : (label==1 ? config.shore_height : y-config.layer_height));
                const auto c=at(layer,i,bottom), d=at(layer,j,bottom);
                const float nx=b[2]-a[2], nz=a[0]-b[0];
                const float length=std::sqrt(nx*nx+nz*nz);
                const point normal={{nx/length,0.f,nz/length}};
                triangle(mesh,a,b,c,normal,soil,silhouette);
                triangle(mesh,b,d,c,normal,soil,silhouette);
                if(label>=0&&!config.debug_layer_colors){
                    const color lower=terrace_inner_color(std::max(0,label-1),config.layers_count);
                    for(size_t v=mesh.vertices.size()-6;v<mesh.vertices.size();++v){
                        const float t=(mesh.vertices[v].position[1]-bottom)/(y-bottom);
                        mesh.vertices[v].color=blend_color(lower,land,t);
                    }
                }
            }
        }
        return mesh;
    }

    std::vector<terrain_mesh_data> terrain_mesh_builder::build_grass(const terrain_mesh_data& surface, const terrain_config& config)
    {
        config.validate();
        std::vector<terrain_mesh_data> batches;
        uint32_t state = config.seed ^ 0xa531bf79u;
        const auto random = [&]() {
            state += 0x9e3779b9u;
            uint32_t value = state;
            value = (value ^ (value >> 16))*0x7feb352du;
            value = (value ^ (value >> 15))*0x846ca68bu;
            value ^= value >> 16;
            return (value & 0xffffffu)/16777216.f;
        };
        constexpr size_t batch_vertices = 60000;
        constexpr float density = 8.3f; // Roots per square metre before patch thinning; doubled from 4.15.
        for (size_t i = 0; i+2 < surface.indices.size(); i += 3)
        {
            const auto& a = surface.vertices.at(surface.indices[i]);
            const auto& b = surface.vertices.at(surface.indices[i+1]);
            const auto& c = surface.vertices.at(surface.indices[i+2]);
            if (a.normal[1] < .99f || b.normal[1] < .99f || c.normal[1] < .99f) continue;
            const int layer = static_cast<int>(std::round(a.position[1]/config.layer_height));
            const int terrace = config.layers_count-layer;
            if (layer < 1 || terrace < 4 || terrace > 7) continue;
            const float ux=b.position[0]-a.position[0], uz=b.position[2]-a.position[2];
            const float vx=c.position[0]-a.position[0], vz=c.position[2]-a.position[2];
            const float expected = std::abs(ux*vz-uz*vx)*.5f*density;
            const int count = static_cast<int>(std::floor(expected+random()));
            for (int blade=0; blade<count; ++blade)
            {
                // Uniform area sampling of the actual smoothed top triangles,
                // with a small inset to keep roots away from terrace edges.
                const float r=std::sqrt(random()), s=random();
                const float wb=.97f*r*(1.f-s)+.01f, wc=.97f*r*s+.01f;
                const float x=a.position[0]+wb*ux+wc*vx;
                const float z=a.position[2]+wb*uz+wc*vz;
                const float patch=.5f+.5f*std::sin(x*.24f+std::sin(z*.13f))*std::cos(z*.21f);
                if(random()>.12f+.88f*patch*patch)continue;
                const float y=a.position[1]+.002f;
                const float angle=random()*6.283185307f;
                const float dx=std::cos(angle), dz=std::sin(angle);
                const float height=(.28f+random()*.27f)*1.33f;
                const float width=.12f+random()*.12f;
                const float lean=(random()-.5f)*height*.65f;
                const float brightness=.85f+random()*.3f;
                const auto tint=[&](float red,float green,float blue) -> color {
                    return {{static_cast<uint8_t>(red*brightness),static_cast<uint8_t>(green*brightness),
                             static_cast<uint8_t>(blue*brightness),255}};
                };
                if (batches.empty() || batches.back().vertices.size()+4 > batch_vertices)
                {
                    batches.emplace_back();
                    batches.back().vertices.reserve(batch_vertices);
                    batches.back().indices.reserve(batch_vertices/4*6);
                }
                auto& mesh=batches.back();
                const uint16_t base=static_cast<uint16_t>(mesh.vertices.size());
                const point normal={{-dz*.6f,.8f,dx*.6f}};
                // Vertex colours enter the existing linear-light terrain path;
                // darker saturated greens survive output gamma without washing out.
                const float vegetation_t=std::max(0.f,std::min(1.f,(layer-1)/3.f));
                const float dry=.12f*(.5f+.5f*std::sin(x*.1f+z*.08f));
                const auto vegetation_color=[&](color low,color high,color dry_color){
                    const auto result=blend_color(blend_color(low,high,vegetation_t),dry_color,dry);
                    return tint(result[0],result[1],result[2]);
                };
                const color root=vegetation_color({{43,60,6,255}},{{16,43,5,255}},{{27,11,4,255}});
                const color tip=vegetation_color({{130,145,24,255}},{{62,122,17,255}},{{67,33,13,255}});
                mesh.vertices.push_back({{{x-dx*width*.5f,y,z-dz*width*.5f}},normal,root});
                mesh.vertices.push_back({{{x+dx*width*.5f,y,z+dz*width*.5f}},normal,root});
                // Full-height card; the fragment mask supplies the pixel staircase.
                mesh.vertices.push_back({{{x-dx*width*.5f-dz*lean,y+height,z-dz*width*.5f+dx*lean}},normal,tip});
                mesh.vertices.push_back({{{x+dx*width*.5f-dz*lean,y+height,z+dz*width*.5f+dx*lean}},normal,tip});
                for(size_t vertex=0;vertex<4;++vertex){
                    mesh.vertices[base+vertex].texcoord={{x,z}};
                    mesh.vertices[base+vertex].grass_bend={{vertex<2?0.f:1.f,height}};
                    // Grass alpha is free: the shader writes the .5 outline sentinel.
                    mesh.vertices[base+vertex].color[3]=vertex%2?255:0;
                }
                for(uint16_t index:{0,1,2,1,3,2})
                    mesh.indices.push_back(static_cast<uint16_t>(base+index));
            }
        }
        return batches;
    }

    std::vector<terrain_tree> terrain_mesh_builder::build_trees(const terrain_mesh_data& surface,const terrain_config& config)
    {
        config.validate();
        std::vector<terrain_tree> result;result.reserve(100);
        uint32_t state=config.seed^0x7154abc9u;
        auto random=[&](){state+=0x9e3779b9u;uint32_t v=state;v=(v^(v>>16))*0x7feb352du;v=(v^(v>>15))*0x846ca68bu;v^=v>>16;return (v&0xffffffu)/16777216.f;};
        struct face {point a,b,c;float min_x,max_x,min_z,max_z;};
        for(int terrace=3;terrace<=7;++terrace){
            const int level=config.layers_count-terrace;if(level<1||level>=config.layers_count)continue;
            const float y=level*config.layer_height;
            std::vector<face> faces;std::vector<float> areas;float total=0;
            for(size_t i=0;i+2<surface.indices.size();i+=3){
                const auto& a=surface.vertices.at(surface.indices[i]);const auto& b=surface.vertices.at(surface.indices[i+1]);const auto& c=surface.vertices.at(surface.indices[i+2]);
                if(a.normal[1]<.99f||b.normal[1]<.99f||c.normal[1]<.99f||std::abs(a.position[1]-y)>.001f)continue;
                const float area=std::abs((b.position[0]-a.position[0])*(c.position[2]-a.position[2])-(b.position[2]-a.position[2])*(c.position[0]-a.position[0]))*.5f;
                if(area<=0)continue;
                faces.push_back({a.position,b.position,c.position,std::min({a.position[0],b.position[0],c.position[0]}),std::max({a.position[0],b.position[0],c.position[0]}),std::min({a.position[2],b.position[2],c.position[2]}),std::max({a.position[2],b.position[2],c.position[2]})});
                total+=area;areas.push_back(total);
            }
            if(faces.empty())continue;
            const auto supported=[&](float x,float z){
                for(const auto& f:faces){
                    if(x<f.min_x||x>f.max_x||z<f.min_z||z>f.max_z)continue;
                    const auto side=[&](point a,point b){return (b[0]-a[0])*(z-a[2])-(b[2]-a[2])*(x-a[0]);};
                    const float a=side(f.a,f.b),b=side(f.b,f.c),c=side(f.c,f.a);
                    if((a>=-.00001f&&b>=-.00001f&&c>=-.00001f)||(a<=.00001f&&b<=.00001f&&c<=.00001f))return true;
                }return false;
            };
            const int count=10+static_cast<int>(random()*11.f);
            const float probabilities[]={.85f,.75f,.5f,.25f,.15f};
            const int pine_count=static_cast<int>(std::round(count*probabilities[terrace-3]));
            int placed=0;
            for(int attempt=0;attempt<20000&&placed<count;++attempt){
                const size_t index=static_cast<size_t>(std::lower_bound(areas.begin(),areas.end(),random()*total)-areas.begin());
                const auto& f=faces[std::min(index,faces.size()-1)];
                const float r=std::sqrt(random()),s=random();
                const auto p=add(f.a,add(scale(subtract(f.b,f.a),r*(1.f-s)),scale(subtract(f.c,f.a),r*s)));
                bool safe=true;
                for(int i=0;i<12;++i){const float angle=i*6.283185307f/12;safe&=supported(p[0]+std::cos(angle)*1.8f,p[2]+std::sin(angle)*1.8f);}
                for(const auto& tree:result)if(tree.terrace==terrace){const float dx=p[0]-tree.position[0],dz=p[2]-tree.position[2];if(dx*dx+dz*dz<36.f)safe=false;}
                if(!safe)continue;
                terrain_tree tree;tree.position={{p[0],y,p[2]}};tree.terrace=terrace;tree.conifer=placed<pine_count;
                tree.height=tree.conifer?6.f+random()*3.f:5.5f+random()*2.5f;
                const uint32_t id=static_cast<uint32_t>(result.size()+1);
                tree.species=static_cast<uint8_t>(tree.conifer?(id%3==0?TreeType::Thuja:TreeType::Pine)
                    :(id%5==0?TreeType::Palm:id%2==0?TreeType::Birch:TreeType::Oak));
                EditorTree logical;logical.id=id;logical.type=static_cast<TreeType>(tree.species);
                tree.yaw=random()*6.283185307f;
                logical.position=tree.position;logical.scale=tree.height/7.f;logical.yaw=tree.yaw;
                tree.mesh=build_voxel_tree(logical,config.seed);
                result.push_back(std::move(tree));++placed;
            }
        }
        return result;
    }

    terrain_mesh_data terrain_mesh_builder::build_water(const terrain_config& config)
    {
        config.validate();
        terrain_mesh_data mesh;
        const float s=config.water_size*.5f, y=config.water_y;
        const point up={{0.f,1.f,0.f}};
        const color blue={{48,153,174,255}};
        triangle(mesh,{{-s,y,-s}},{{-s,y,s}},{{s,y,-s}},up,blue);
        triangle(mesh,{{s,y,-s}},{{-s,y,s}},{{s,y,s}},up,blue);
        return mesh;
    }

    terrain_mesh_data terrain_mesh_builder::build_seabed(const terrain_config& config)
    {
        auto mesh = build_water(config);
        for (auto& vertex : mesh.vertices)
        {
            vertex.position[1] = -(config.underwater_layers_count+1)*config.layer_height;
            vertex.color = {{137,135,100,255}};
            vertex.editor_palette = {{0.f,0.f}};
        }
        return mesh;
    }
}
