/* Use invoke-hand-blockbench.mjs with an isolated desktop Blockbench instance.
 * Authors the editable model and native animation through Blockbench APIs.
 */
(async () => {
    try {
        newProject(Formats.free);
        Project.name = 'Hand Cursor'; Project.texture_width = Project.texture_height = 32;
        // Small, deliberately visible pixels, using a restrained sandstone palette.
        const canvas = document.createElement('canvas'); canvas.width = canvas.height = 32;
        const ctx = canvas.getContext('2d');
        const palette = ['#c68a53', '#e7b77e', '#f3cd96', '#a86f46', '#494442', '#63594e', '#22c4d2', '#83e9e6'];
        for (let y=0;y<32;y++) for(let x=0;x<32;x++) {
            const patch = Math.floor(x/8), row = Math.floor(y/16);
            const color = row*4+patch;
            ctx.fillStyle = palette[color]; ctx.fillRect(x,y,1,1);
            if ((x*13+y*7)%17===0) { ctx.fillStyle='rgba(255,239,203,.12)';ctx.fillRect(x,y,1,1); }
        }
        new Texture({name:'hand_cursor.png'}).fromDataURL(canvas.toDataURL()).add(false);
        const root = new Group({name:'hand_root',origin:[-3.2,24.8,-1]}).init();
        const palm = new Group({name:'palm',origin:[0,5,0]}).addTo(root).init();
        const cube = (name, a, b, parent, swatch=1) => {
            const e = new Cube({name,from:a,to:b,autouv:0}).addTo(parent).init();
            const x=(swatch%4)*8,y=Math.floor(swatch/4)*16;
            for (const face of Object.values(e.faces)) {face.texture=Texture.all[0].uuid;face.uv=[x,y,x+8,y+8];}
            return e;
        };
        cube('palm_center',[-4.8,.6,-2],[4.8,9.5,2],palm);
        cube('palm_left_bevel',[-6,1.7,-1.4],[-4.8,8.4,1.4],palm,0);
        cube('palm_right_bevel',[4.8,1.7,-1.4],[6,8.4,1.4],palm,2);
        cube('palm_heel',[-3.8,-.2,-1.6],[3.8,.6,1.6],palm,0);
        cube('palm_back_plate',[-3.8,2.2,2],[3.8,8,2.5],palm,2);
        cube('palm_crease',[-4.3,3.1,-2.15],[3.8,4.0,-2],palm,0);
        const cuff = new Group({name:'cuff',origin:[0,0,0]}).addTo(palm).init();
        cube('wrist',[-3.6,-2,-1.65],[3.6,0,1.65],cuff,3);
        cube('cuff_body',[-4.8,-4.8,-2.6],[4.8,-2,2.6],cuff,4);
        cube('cuff_front_rim',[-5.2,-2.7,-2.9],[5.2,-2,2.9],cuff,5);
        cube('cuff_back_rim',[-4.4,-5.6,-2.3],[4.4,-4.8,2.3],cuff,5);
        cube('cyan_front_setting',[-2.4,-4.6,-2.9],[2.4,-2.9,-2.6],cuff,5);
        cube('cyan_front_gem',[-1.7,-4.3,-3.1],[1.7,-3.1,-2.9],cuff,6);
        cube('cyan_back_gem',[-1.7,-4.3,2.6],[1.7,-3.1,2.9],cuff,7);
        cube('cyan_side_gem',[4.8,-4.2,-1],[5,-3,1],cuff,6);
        const fingers = [];
        function finger(name,x,y,width,lengths,angles,parent=palm,z=0) {
            let p=parent, offset=y;
            const names=[name+'_base',name+'_middle',name+'_tip'];
            for(let i=0;i<3;i++) {
                const g = new Group({name:names[i],origin:[x,offset,z],rotation:[angles[i],0,0]}).addTo(p).init();
                cube(names[i]+'_joint',[x-width*.43,offset-.15,z-1.1],[x+width*.43,offset+.7,z+1.1],g,3);
                cube(names[i]+'_segment',[x-width*.5,offset+.5,z-1.45],[x+width*.5,offset+lengths[i]-.45,z+1.45],g,1);
                cube(names[i]+'_cap',[x-width*.38,offset+lengths[i]-.45,z-1.1],[x+width*.38,offset+lengths[i],z+1.1],g,2);
                offset+=lengths[i];p=g;
            }
            fingers.push(p);
        }
        finger('index',-3.2,9.5,3.0,[6,5,4.3],[0,0,0]);
        finger('middle',.25,9.3,3.0,[4.8,4,3.5],[-68,-78,-48]);
        finger('ring',3.6,8.8,2.8,[4.5,3.7,3.2],[-75,-80,-46]);
        finger('little',6.0,7.5,2.35,[3.9,3.1,2.7],[-83,-85,-42]);
        const thumb = new Group({name:'thumb_splay',origin:[-4.8,2.4,0],rotation:[-15,0,-55]}).addTo(palm).init();
        finger('thumb',-4.8,2.4,3.2,[3.6,3.4,2.8],[-30,-65,-45],thumb);
        const idle = new Animation({name:'idle',loop:'loop',length:2.8,snapping:20}).add();
        const poses=[[0,0,0],[.8,0,1],[0,.6,0],[-.7,0,-.8],[0,-.5,0],[.7,0,.8],[0,.4,0],[0,0,0]];
        for(let i=0;i<8;i++) {
            const time=i*.4;
            idle.getBoneAnimator(root).addKeyframe({channel:'rotation',time,interpolation:'catmullrom',data_points:[{x:poses[i][0],y:poses[i][1],z:poses[i][2]}]});
            // Nonnegative bob keeps the authored fingertip above its hover floor.
            idle.getBoneAnimator(root).addKeyframe({channel:'position',time,interpolation:'linear',data_points:[{x:0,y:[0,.3,.15,0,.1,.27,.12,0][i],z:0}]});
            for(let f=1;f<fingers.length;f++) idle.getBoneAnimator(fingers[f]).addKeyframe({channel:'rotation',time,interpolation:'catmullrom',data_points:[{x:i===0||i===7?0:Math.sin(i*1.1+f)*1.2,y:0,z:0}]});
        }
        idle.select(); Modes.options.animate.select(); Timeline.time=0; Animator.preview();
        const output = Codecs.project.compile();
        Project.save_path='C:/Hobby/game-box/gbResources/hand_cursor.bbmodel';Project.saved=true;
        Canvas.updateAll();Animator.showDefaultPose();Project.model_3d.updateMatrixWorld(true);
        const rest=Group.all.map(g=>({name:g.name,matrix:g.mesh.matrixWorld.toArray()}));
        const frames=[];
        for(let f=0;f<=84;f++) {Timeline.time=f/30;Animator.preview();Project.model_3d.updateMatrixWorld(true);frames.push({time:f/30,bones:Group.all.map(g=>({name:g.name,matrix:g.mesh.matrixWorld.toArray()}))});}
        Timeline.time=0;Animator.preview();
        return {model:output,texture:canvas.toDataURL().split(',')[1],bake:{fps:30,rest,frames},report:{ok:true,version:Blockbench.version,cubes:Cube.all.length,groups:Group.all.length,animation:idle.name}};
    } catch(e) {throw e;}
})();
