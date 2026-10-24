/* Native Blockbench free-format authoring. Execute with invoke-party-liner.mjs.
 * One model unit = one planned Game Box world unit. No runtime scene edits.
 */
(async () => {
    if (typeof Formats === 'undefined') throw Error('Blockbench has not initialized');
    newProject(Formats.free);
    Project.name = 'Rhythm Party Liner';
    Project.texture_width = Project.texture_height = 128;
    const palette = [
        ['hull','#18212d'], ['hull_alt','#202c39'], ['keel','#812e3d'], ['keel_alt','#9e3848'],
        ['ivory','#eee6d5'], ['ivory_shadow','#c7c8c0'], ['teak','#b98a59'], ['teak_light','#cda773'],
        ['gold','#cda45b'], ['dark_metal','#263446'], ['window','#173d50'], ['window_glow','#ffe1a1'],
        ['funnel','#d79a42'], ['funnel_shade','#b57c32'], ['black','#101826'], ['speaker','#28273a'],
        ['magenta','#ed4bae'], ['cyan','#4ddce7'], ['violet','#9264df'], ['pink_light','#ff9fdf'],
        ['floor_dark','#35394f'], ['floor_blue','#73c9df'], ['floor_pink','#d57ec8'], ['floor_white','#dacdf0'],
        ['leaf','#467641'], ['leaf_light','#73964d'], ['trunk','#84603f'], ['cushion','#bd634b'],
        ['red','#bd424e'], ['brass_shadow','#8c6b3e'], ['warm_light','#ffe9ba'], ['screen_dark','#241a42'],
        ['sun_orange','#ffab58'], ['screen_purple','#633e88'], ['waterline','#0f1825'], ['deck_trim','#e5d7b5'],
        ['cream','#fff1cd'], ['cap_highlight','#354153'], ['lifeboat','#f6f0dd'], ['lifeboat_inside','#8e6550']
    ];
    const swatches = Object.fromEntries(palette.map(([name],i)=>[name,i]));
    const canvas = document.createElement('canvas'); canvas.width = canvas.height = 128;
    const ctx = canvas.getContext('2d');
    palette.forEach(([name,color],i)=>{
        const x=i%8*16,y=Math.floor(i/8)*16;
        ctx.fillStyle=color;ctx.fillRect(x,y,16,16);
        // Very restrained pixel grain; lights and screen colors stay flat.
        if(!/light|glow|magenta|cyan|violet|screen/.test(name)) {
            for(let k=0;k<14;k++) {
                ctx.fillStyle=k%2?'rgba(255,255,255,.035)':'rgba(0,0,0,.04)';
                ctx.fillRect(x+(k*7+3)%14+1,y+(k*11+5)%14+1,1,1);
            }
        }
    });
    const texture = new Texture({name:'palette.png'}).fromDataURL(canvas.toDataURL()).add(false);
    await new Promise((resolve,reject)=>{
        const started=Date.now();
        const poll=()=>texture.width===128 && texture.height===128 ? resolve() :
            Date.now()-started>5000 ? reject(Error('Palette texture did not load')) : setTimeout(poll,20);
        poll();
    });
    const root = new Group({name:'rhythm_party_liner',origin:[0,0,0]}).init();
    const groups = {};
    const group = (name,parent=root) => {
        const g=new Group({name,origin:[0,0,0]}).addTo(parent).init(); groups[name]=g;return g;
    };
    for(const name of ['hull','decks','superstructure','funnels','lifeboats','railings','lamps',
        'dance_floor','stage','dj_console','speakers','screen','stairs','props']) group(name);
    const emissive=group('emissive_surfaces');
    const glow={};
    for(const name of ['warm_windows','festoon_bulbs','dance_tiles','stage_neon','console_leds'])
        glow[name]=group(name,emissive);
    let serial=0;
    function box(name,from,to,color,parent,rotation) {
        if(from.some((v,i)=>!Number.isFinite(v)||v>=to[i]))throw Error('Degenerate box '+name);
        const e=new Cube({name:`${name}_${++serial}`,from,to,autouv:0,origin:from.map((v,i)=>(v+to[i])/2),rotation:rotation||[0,0,0]}).addTo(parent).init();
        const index=swatches[color]; if(index===undefined)throw Error('Unknown swatch '+color);
        const x=index%8*16,y=Math.floor(index/8)*16;
        for(const face of Object.values(e.faces)){face.texture=texture.uuid;face.uv=[x+1,y+1,x+15,y+15];}
        return e;
    }
    function centered(name,x,y,z,w,h,d,c,g) {
        return box(name,[x-w/2,y-h/2,z-d/2],[x+w/2,y+h/2,z+d/2],c,g);
    }
    function beam(name,a,b,width,c,g) {
        const delta=b.map((v,i)=>v-a[i]),len=Math.hypot(...delta),center=a.map((v,i)=>(v+b[i])/2);
        const e=centered(name,...center,width,len,width,c,g);
        e.rotation=[Math.atan2(delta[2],Math.hypot(delta[0],delta[1]))*180/Math.PI,0,
            -Math.atan2(delta[0],delta[1])*180/Math.PI];
        return e;
    }
    const halfWidth=z => z < -136 ? 4+(z+160)/24*27 : z>26 ? 31-(z-26)/24*17 : 31;
    // Four-metre hull sections give the silhouette a clean deliberate voxel taper.
    for(let z=-160;z<50;z+=4) {
        const end=Math.min(z+4,50),w=Math.min(halfWidth(z),halfWidth(end));
        box('red_lower_hull',[-w+1,-16,z],[w-1,-10.5,end],Math.floor(z/4)%3?'keel':'keel_alt',groups.hull);
        box('black_hull',[-w,-10.5,z],[w,2.4,end],Math.floor(z/4)%4?'hull':'hull_alt',groups.hull);
        box('teak_deck',[-w,2.4,z],[w,3.55,end],Math.floor(z/4)%3?'teak':'teak_light',groups.decks);
        for(const sign of [-1,1]) {
            box('ivory_gunwale',[sign<0?-w:-.25+w,2.35,z],[sign<0?.25-w:w,3.1,end],'ivory',groups.hull);
            centered('waterline',sign*(w-.25),-9.8,(z+end)/2,.32,.5,end-z,'waterline',groups.hull);
            centered('stern_cyan_trim',sign*(w-.1),2.8,(z+end)/2,.3,.28,end-z,'cyan',glow.stage_neon);
        }
    }
    // Long warm porthole rows, with dark metal surrounds on the upper row.
    for(const side of [-1,1])for(let z=-146;z<40;z+=5.5) {
        const w=halfWidth(z);
        centered('porthole_frame',side*(w+.06),-2.0,z,.32,1.25,1.15,'black',groups.hull);
        centered('porthole_glass',side*(w+.24),-2.0,z,.12,.72,.64,'window_glow',glow.warm_windows);
        if(z>-135 && z<24)centered('lower_porthole',side*(w+.12),-6.5,z,.16,.58,.56,'window_glow',glow.warm_windows);
    }
    // Promenade building stays behind the stage; the dance floor remains open.
    box('promenade_base',[-25,3.55,-145],[25,11.5,-39],'ivory_shadow',groups.superstructure);
    box('promenade_face',[-24.7,4.2,-144.8],[24.7,10.9,-38.8],'ivory',groups.superstructure);
    box('promenade_window_band',[-25.1,6.8,-140],[25.1,9.5,-40],'window',groups.superstructure);
    box('upper_walk_deck',[-28,11.5,-147],[28,12.2,-36],'ivory',groups.decks);
    box('upper_teak',[-27.6,12.2,-146.6],[27.6,12.4,-36.4],'teak_light',groups.decks);
    box('upper_accommodation',[-20.5,12.4,-140],[20.5,19,-48],'ivory',groups.superstructure);
    box('boat_deck_roof',[-22.5,19,-142],[22.5,19.8,-46],'ivory',groups.decks);
    box('boat_deck_teak',[-22.1,19.8,-141.6],[22.1,20,-46.4],'teak',groups.decks);
    for(const side of [-1,1])for(let z=-139;z<-43;z+=5.2) {
        centered('promenade_pillar',side*25.35,7.45,z,.9,7.7,.85,'ivory',groups.superstructure);
        centered('promenade_amber',side*25.3,9.8,z+2,.28,.5,.58,'warm_light',glow.warm_windows);
        centered('cabin_window_frame',side*20.6,16.1,z,.25,2.0,1.6,'dark_metal',groups.superstructure);
        centered('cabin_window',side*20.78,16.1,z,.15,1.4,1.0,'window_glow',glow.warm_windows);
    }
    // Bridge steps down towards the stage, with a broad dark horizontal visor.
    box('bridge_lower',[-19,12.4,-54],[19,20.5,-36.5],'ivory',groups.superstructure);
    box('bridge_glazing',[-18.8,17.3,-36.7],[18.8,19.7,-36.25],'window',groups.superstructure);
    for(let x=-18;x<=18;x+=4)centered('bridge_mullion',x,18.5,-36,.35,2.8,.5,'ivory',groups.superstructure);
    box('bridge_roof',[-21,20.5,-55],[21,21.4,-35.2],'ivory',groups.decks);
    box('bridge_roof_teak',[-20.5,21.4,-54.5],[20.5,21.6,-35.7],'teak_light',groups.decks);
    box('roof_hatch',[-5,21.6,-49],[5,23.6,-42],'ivory',groups.superstructure);
    // Three broad octagonal voxel funnels: economical stepped cross-sections.
    for(const [i,z] of [-66,-96,-126].entries()) {
        const g=group(`funnel_${i+1}`,groups.funnels);
        centered('funnel_plinth',0,20.6,z,16,1.2,18,'dark_metal',g);
        for(const [x,width,depth] of [[0,12,14],[-6.5,1,10],[6.5,1,10]]) {
            centered('ochre_stack',x,29,z,width,17,depth,'funnel',g);
            centered('dark_cap',x,39,z,width,4,depth,'black',g);
        }
        for(const x of [-4,0,4])centered('stack_vertical_seam',x,29,z+7.03,.11,16.7,.12,'funnel_shade',g);
        for(let y=23;y<38;y+=4)centered('stack_horizontal_seam',0,y,z+7.06,11.8,.1,.13,'funnel_shade',g);
        centered('cap_opening',0,41.05,z,10,.12,10,'black',g);
    }
    // Lifeboats along both boat-deck edges, clear of the bridge.
    for(const side of [-1,1])for(const z of [-61,-81,-101,-121]) {
        const g=group(`lifeboat_${side<0?'port':'starboard'}_${-z}`,groups.lifeboats),x=side*25;
        centered('boat_cradle',x,12.9,z,4.6,.7,11.5,'dark_metal',g);
        centered('boat_keel',x,13.7,z,2.6,.9,8.6,'lifeboat',g);
        centered('boat_hull',x,14.4,z,4,1.0,10.5,'lifeboat',g);
        centered('boat_interior',x,14.95,z,2.9,.12,8.5,'lifeboat_inside',g);
        for(const dz of [-3,0,3])centered('boat_bench',x,15.1,z+dz,3.3,.35,.7,'ivory',g);
        for(const dz of [-4.7,4.7]) {
            beam('davit',[x-side*3,12.5,z+dz],[x-side*3,17,z+dz],.38,'ivory',g);
            beam('davit_arm',[x-side*3,17,z+dz],[x,17,z+dz],.38,'ivory',g);
            beam('davit_rope',[x,17,z+dz],[x,15.2,z+dz],.12,'gold',g);
        }
    }
    function rail(a,b,y,parent=groups.railings,spacing=4) {
        const length=Math.hypot(b[0]-a[0],b[1]-a[1]),n=Math.max(1,Math.ceil(length/spacing));
        for(const height of [.85,1.75])beam('railing_bar',[a[0],y+height,a[1]],[b[0],y+height,b[1]],.22,'gold',parent);
        for(let i=0;i<=n;i++){
            const x=a[0]+(b[0]-a[0])*i/n,z=a[1]+(b[1]-a[1])*i/n;
            centered('railing_post',x,y+1.0,z,.32,2,.32,'gold',parent);
        }
    }
    for(const side of [-1,1]) {
        for(let z=-160;z<50;z+=8) {
            const end=Math.min(z+8,50);
            rail([side*(halfWidth(z)-.7),z],[side*(halfWidth(end)-.7),end],3.55);
        }
        rail([side*27.3,-145],[side*27.3,-38],12.4);
        rail([side*21.8,-140],[side*21.8,-47],20);
    }
    rail([-13.3,49.4],[13.3,49.4],3.55);
    rail([-20.2,-35.9],[20.2,-35.9],21.6);
    // The open 36 x 48 play area includes the existing crowd anchors.
    box('dancefloor_foundation',[-19,3.55,-21],[19,3.78,29],'floor_dark',groups.dance_floor);
    for(let iz=0;iz<16;iz++)for(let ix=0;ix<12;ix++) {
        const x=-18+ix*3,z=-20+iz*3;
        const color=['floor_pink','floor_blue','floor_white','violet'][(ix+iz*3)%4];
        box('dance_tile',[x+.09,3.78,z+.09],[x+2.91,3.86,z+2.91],color,glow.dance_tiles);
    }
    for(const x of [-19,19])centered('dancefloor_neon_edge',x,3.9,4,.28,.18,50,'cyan',glow.stage_neon);
    // Raised stage at the original DJ end; no pillars inside the floor.
    box('stage_black_base',[-18,3.55,-38],[18,6.35,-24],'black',groups.stage);
    box('stage_front_facade',[-17.8,3.8,-24.05],[17.8,6.05,-23.8],'speaker',groups.stage);
    for(let x=-16;x<17;x+=4)centered('stage_front_pixel',x,4.85,-23.65,3.7,1.9,.12,x%8?'magenta':'violet',glow.stage_neon);
    box('stage_teak',[-18.2,6.35,-38.2],[18.2,6.55,-23.8],'floor_dark',groups.stage);
    for(const x of [-17.5,17.5])centered('stage_truss_column',x,12,-36,1,11,1,'dark_metal',groups.stage);
    centered('stage_truss_header',0,17.5,-36,36,1,1,'dark_metal',groups.stage);
    for(const x of [-17.45,17.45])centered('truss_neon',x,12,-35.35,.35,10,.25,x<0?'magenta':'cyan',glow.stage_neon);
    // Pixel sunset LED wall, built from editable layered blocks.
    box('screen_frame',[-12.6,8.7,-36],[12.6,16.7,-34.8],'black',groups.screen);
    box('screen_panel',[-12,9.2,-34.7],[12,16.2,-34.5],'screen_dark',groups.screen);
    for(const x of [-12.25,12.25])centered('screen_neon_border',x,12.7,-34.35,.25,7.5,.18,'magenta',glow.stage_neon);
    for(let row=0;row<6;row++) {
        const width=[7.2,8.5,8.6,7.6,6,3.6][row];
        centered('pixel_sunset',0,11.1+row*.65,-34.28,width,.48,.15,row<2?'pink_light':'sun_orange',glow.stage_neon);
    }
    for(let row=0;row<4;row++)centered('sunset_horizon',0,10.5-row*.35,-34.26,13+row*2,.13,.17,'cyan',glow.stage_neon);
    // Console is separate from the scenic screen, including visible top controls.
    box('dj_counter_base',[-6.5,6.55,-31.9],[6.5,8.8,-28.3],'black',groups.dj_console);
    box('dj_counter_top',[-7,8.8,-32.3],[7,9.1,-27.9],'dark_metal',groups.dj_console);
    box('dj_counter_face',[-6.2,6.85,-28.25],[6.2,8.5,-28.05],'screen_purple',groups.dj_console);
    for(const x of [-4.1,4.1]) {
        centered('turntable_base',x,9.16,-30,3.8,.16,3.2,'black',groups.dj_console);
        centered('turntable_platter',x,9.32,-30,2.15,.16,2.15,'cap_highlight',groups.dj_console);
        centered('turntable_label',x,9.43,-30,.7,.08,.7,'cyan',glow.console_leds);
        for(let k=0;k<3;k++)centered('deck_button',x-1+k*.65,9.32,-28.75,.32,.14,.32,k%2?'magenta':'cyan',glow.console_leds);
    }
    centered('mixer_body',0,9.25,-30,2.8,.3,3,'speaker',groups.dj_console);
    for(const x of [-.85,0,.85])for(let k=0;k<3;k++)centered('mixer_knob',x,9.5,-30.9+k*.7,.26,.3,.26,'gold',groups.dj_console);
    centered('console_front_strip',0,7.6,-27.95,8,.25,.15,'cyan',glow.console_leds);
    for(const side of [-1,1]) {
        const g=group(side<0?'speaker_left':'speaker_right',groups.speakers),x=side*14.7;
        box('speaker_cabinet',[x-1.75,6.55,-34.3],[x+1.75,16,-31.8],'black',g);
        for(const y of [8.4,11.5,14.5]) {
            centered('speaker_cone_frame',x,y,-31.65,2.35,2.35,.35,'speaker',g);
            centered('speaker_cone',x,y,-31.42,1.65,1.65,.18,'cap_highlight',g);
            centered('speaker_dust_cap',x,y,-31.27,.75,.75,.18,'black',g);
        }
        centered('speaker_power_led',x,7,-31.1,.35,.15,.12,'magenta',glow.console_leds);
        centered('subwoofer',side*12.3,7.55,-26,4,2,3,'speaker',g);
    }
    // Side stairs lead onto stage without intruding into the 36-wide dance floor.
    for(const side of [-1,1]) {
        for(let k=0;k<6;k++) {
            box('stage_stair',[side<0?-25:18,3.55,-24-k*1.1],[side<0?-18:25,4.05+k*.5,-22.9-k*1.1],'teak_light',groups.stairs);
        }
        rail([side*25,-22.9],[side*25,-29.5],5.1,groups.stairs,2);
        // Upper promenade stairs beside the stage (outside x=18).
        for(let k=0;k<12;k++)box('promenade_step',[side<0?-29:25,3.55,-35-k*.75],[side<0?-25:29,4.25+k*.7,-34.25-k*.75],'ivory',groups.stairs);
    }
    function lamp(x,y,z) {
        centered('lamp_foot',x,y+.1,z,.85,.2,.85,'dark_metal',groups.lamps);
        centered('lamp_post',x,y+1.55,z,.25,3.1,.25,'gold',groups.lamps);
        centered('lamp_lantern_frame',x,y+3.2,z,.85,1,.85,'gold',groups.lamps);
        centered('lamp_glow',x,y+3.25,z,.65,.65,.65,'warm_light',glow.festoon_bulbs);
        centered('lamp_cap',x,y+3.85,z,1,.22,1,'dark_metal',groups.lamps);
    }
    for(const side of [-1,1])for(let z=-150;z<45;z+=12)lamp(side*(halfWidth(z)-1.2),3.55,z);
    for(const side of [-1,1])for(let z=-137;z<-42;z+=16)lamp(side*26.3,12.4,z);
    function palm(x,z) {
        const g=groups.props;
        centered('palm_planter',x,4.4,z,3.4,1.7,3.4,'ivory',g);
        centered('palm_soil',x,5.3,z,2.8,.14,2.8,'lifeboat_inside',g);
        centered('palm_trunk',x,8,z,.75,5.4,.75,'trunk',g);
        centered('palm_crown',x,10.65,z,2.2,1.3,2.2,'leaf',g);
        for(const [dx,dz] of [[1,0],[-1,0],[0,1],[0,-1]]) {
            centered('palm_frond_upper',x+dx*1.7,10.6,z+dz*1.7,dx?3:1.4,.8,dz?3:1.4,'leaf_light',g);
            centered('palm_frond_lower',x+dx*3,9.85,z+dz*3,dx?1.8:1.2,1,dz?1.8:1.2,'leaf',g);
        }
    }
    for(const x of [-23,23])for(const z of [-16,22])palm(x,z);
    // Low benches and front capstan sit outside the playable deck.
    for(const side of [-1,1])for(const z of [-7,7,34]) {
        centered('bench_frame',side*26,4.05,z,3,.8,6,'ivory',groups.props);
        centered('bench_cushion',side*26,4.65,z,2.8,.4,5.8,'cushion',groups.props);
    }
    for(const x of [-7,7])centered('stern_mooring',x,4.35,39,2.4,1.6,2.4,'dark_metal',groups.props);
    centered('bow_capstan',0,4.25,-150,3,1.4,3,'dark_metal',groups.props);
    // Fore and bridge masts; thin stays and warm strings describe the silhouette.
    for(const [z,y] of [[-148,3.55],[-46,21.6]]) {
        centered('mast',0,y+14,z,.65,28,.65,'gold',groups.props);
        centered('mast_dark_tip',0,y+28.5,z,.5,1,.5,'black',groups.props);
        centered('mast_crossbar',0,y+18,z,13,.4,.4,'gold',groups.props);
        for(const x of [-19,19])beam('mast_stay',[0,y+25,z],[x,y,z+4],.12,'dark_metal',groups.props);
    }
    // Festoon paths with real sag. Small cubes are bulbs, never point-light promises.
    function festoon(a,b,sag,n) {
        let previous=a;
        for(let i=1;i<=n;i++) {
            const u=i/n,p=a.map((v,k)=>v+(b[k]-v)*u);p[1]-=Math.sin(Math.PI*u)*sag;
            beam('festoon_cable',previous,p,.11,'dark_metal',groups.lamps);
            centered('festoon_bulb',...p,.4,.5,.4,'warm_light',glow.festoon_bulbs);previous=p;
        }
    }
    festoon([0,31.5,-148],[0,49.6,-46],5.5,30);
    for(const side of [-1,1]) {
        festoon([0,46,-46],[side*27,7.8,34],5,22);
        festoon([side*25,16,-138],[side*25,16,-46],3,24);
    }
    Canvas.updateAll();
    Project.model_3d.updateMatrixWorld(true);
    const bounds=new THREE.Box3().setFromObject(Project.model_3d);
    // Native renderer-independent report; the saved codec remains authoritative.
    const report={version:Blockbench.version,format:Format.id,cubes:Cube.all.length,
        triangles:Cube.all.reduce((sum,e)=>sum+Object.values(e.faces).filter(f=>f.texture!==null).length*2,0),
        groups:Group.all.map(g=>({name:g.name,cubes:g.children.filter(e=>e instanceof Cube).length})),
        bounds:{min:bounds.min.toArray(),max:bounds.max.toArray()},
        palette:palette.map(([name,color],i)=>({name,color,index:i})),
        units:'1 Blockbench unit = 1 planned game world unit',
        deck_y:3.55,dance_surface_y:3.86,play_area:{x:[-18,18],z:[-20,28]},
        stage:{x:[-18,18],z:[-38,-24],surface_y:6.55},
        note:'Static art only; emission groups are material annotations, not runtime lights.'};
    const model=Codecs.project.compile();
    Project.save_path='C:/Hobby/game-box/gbResources/rhythm_party_liner.bbmodel';Project.saved=true;
    return {model,texture:canvas.toDataURL().split(',')[1],report};
})();
