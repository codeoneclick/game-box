/* Update only animations on the existing Blockbench model. Mesh, UV, texture,
 * hierarchy and bind pose are preserved; invoke through invoke-hand-blockbench.mjs.
 */
(async()=>{
    const source=globalThis.handCursorSource;
    Codecs.project.load(source,{path:'C:/Hobby/game-box/gbResources/hand_cursor.bbmodel',no_file:true});
    for(const animation of Animation.all.slice())animation.remove();
    const idle=new Animation({name:'idle',loop:'loop',length:2.8,snapping:30}).add();
    const byName=Object.fromEntries(Group.all.map(g=>[g.name,g]));
    const splay={index:5,middle:0,ring:-4,little:-9};
    const add=(g,channel,time,value)=>idle.getBoneAnimator(g).addKeyframe({channel,time,interpolation:'catmullrom',data_points:[{x:value[0],y:value[1],z:value[2]}]});
    for(let i=0;i<8;i++){
        const time=i*.4,phase=2*Math.PI*i/7;
        const wave=i===7?0:Math.sin(phase),breath=i===7?0:(1-Math.cos(phase))*.5;
        add(byName.hand_root,'rotation',time,[2*wave,.5*wave,.6*wave]);
        add(byName.hand_root,'position',time,[.08*wave,.3*wave,.45*breath]);
        add(byName.palm,'rotation',time,[.8*wave,0,0]);
        for(const finger of ['index','middle','ring','little','thumb']){
            for(const [j,joint] of ['base','middle','tip'].entries()){
                const g=byName[finger+'_'+joint];
                // Animation is additive to the old pointing bind pose.
                const flex=1.1*wave+(j===0?0:1.4*breath);
                add(g,'rotation',time,[-g.rotation[0]+flex,-g.rotation[1],-g.rotation[2]+(j===0&&finger!=='thumb'?splay[finger]:0)]);
            }
        }
        const thumb=byName.thumb_splay;
        add(thumb,'rotation',time,[-thumb.rotation[0]-10+wave,-thumb.rotation[1],-thumb.rotation[2]+45]);
    }
    idle.select();Modes.options.animate.select();Timeline.time=0;Animator.preview();
    const compiled=JSON.parse(Codecs.project.compile());
    // Keep all non-animation source properties semantically identical.
    const model={...source,animations:compiled.animations};
    Animator.showDefaultPose();Project.model_3d.updateMatrixWorld(true);
    const rest=Group.all.map(g=>({name:g.name,matrix:g.mesh.matrixWorld.toArray()})),frames=[];
    for(let frame=0;frame<=84;frame++){
        Timeline.time=frame/30;Animator.preview();Project.model_3d.updateMatrixWorld(true);
        frames.push({time:frame/30,bones:Group.all.map(g=>({name:g.name,matrix:g.mesh.matrixWorld.toArray()}))});
    }
    Timeline.time=0;Animator.preview();Project.saved=true;
    return {model,texture:source.textures[0].source.split(',')[1],bake:{fps:30,rest,frames},report:{ok:true,version:Blockbench.version,cubes:Cube.all.length,groups:Group.all.length,animation:'idle',pose:'open_palm',mesh_unchanged:true}};
})();
