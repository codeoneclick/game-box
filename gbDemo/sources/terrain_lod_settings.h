#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
namespace game {
struct TerrainLodSettings {
 int lodCount=5;
 float contourSimplificationPercent=0;
 bool wireframe=false;
 bool wireframe_available=false; // Runtime capability, not a saved preference.
 float nearErrorPx=.5f,normalErrorPx=1.5f,farErrorPx=5.f;
 int debugMode=0; // 0 material, 1 actual reduction, 2 contour wire, 3 screen error, 4 targets, 5 active contour points.
 uint64_t revision=0;
 bool set_lod_count(int n){n=std::max(1,std::min(8,n));if(n==lodCount)return false;lodCount=n;++revision;return true;}
 bool set_contour_percent(float p){if(!std::isfinite(p))return false;p=std::max(0.f,std::min(90.f,p));if(p==contourSimplificationPercent)return false;contourSimplificationPercent=p;++revision;return true;}
 float contour_percent(int level)const{return lodCount<=1?0.f:contourSimplificationPercent*std::max(0,std::min(lodCount-1,level))/float(lodCount-1);}
 bool valid()const{return lodCount>=1&&lodCount<=8&&std::isfinite(contourSimplificationPercent)&&contourSimplificationPercent>=0&&contourSimplificationPercent<=90&&std::isfinite(nearErrorPx)&&std::isfinite(normalErrorPx)&&std::isfinite(farErrorPx)&&nearErrorPx>=.05f&&nearErrorPx<=normalErrorPx&&normalErrorPx<=farErrorPx&&farErrorPx<=12&&debugMode>=0&&debugMode<=5;}
};
}
