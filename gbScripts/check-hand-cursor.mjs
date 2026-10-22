// Authored source checks; FBX and runtime checks complement this validation.
import fs from 'node:fs';
import assert from 'node:assert/strict';
import crypto from 'node:crypto';
const model = JSON.parse(fs.readFileSync('gbResources/hand_cursor.bbmodel', 'utf8'));
assert.equal(model.meta.model_format, 'free');
assert(model.elements.length >= 25 && model.elements.length < 180);
assert(!model.elements.some(e => /platform|ground|baseplate/i.test(e.name)));
const groups = new Map(model.groups.map(g => [g.uuid, g]));
const digest=value=>crypto.createHash('sha256').update(value).digest('hex');
assert.equal(digest(fs.readFileSync('gbBundle/Meshes/hand_cursor.fbx')),'246331d7906b18333ab48107c31c7848044b1cb2b62c67afc48b44d9ba4cdc4f','Base mesh changed');
assert.equal(digest(fs.readFileSync('gbBundle/Textures/hand_cursor.png')),'c5115f6df19c47cdc403817c328f0706d731ae43a36d63af28fd1dd896c76ccd','Texture changed');
assert.equal(digest(JSON.stringify({elements:model.elements,groups:model.groups,outliner:model.outliner,textures:model.textures})),'29914aad3c2cadc27694c9c46990818e52e6979ab0c4a83c563cf81ebf6fea76','Source geometry/rig changed');
for (const finger of ['index', 'middle', 'ring', 'little', 'thumb'])
    assert(model.groups.some(g => g.name === finger + '_tip'), finger + ' joint absent');
const idle = model.animations.find(a => a.name === 'idle');
assert(idle && idle.loop === 'loop' && idle.length >= 2);
const root = model.groups.find(g => g.name === 'hand_root');
const keys = idle.animators[root.uuid].keyframes.filter(k => k.channel === 'rotation');
assert.equal(keys.length, 8);
assert.deepEqual(keys[0].data_points, keys[7].data_points);
assert(keys.some(k => JSON.stringify(k.data_points) !== JSON.stringify(keys[0].data_points)));
const png = Buffer.from(model.textures[0].source.split(',')[1], 'base64');
assert(png.equals(fs.readFileSync('gbBundle/Textures/hand_cursor.png')));
assert(png.readUInt32BE(16) <= 64 && png.readUInt32BE(20) <= 64);
for (const e of model.elements) {
    assert(e.type === 'cube' && Object.values(e.faces).every(f => f.texture === 0));
    assert(e.from.every((v, i) => v < e.to[i]));
}
assert(fs.statSync('gbBundle/Meshes/hand_cursor.fbx').size > 10000);
assert(fs.statSync('gbBundle/Meshes/hand_cursor_idle.fbx').size > 10000);
const bake=JSON.parse(fs.readFileSync('gbResources/hand_cursor.animation.json'));
assert.equal(bake.frames.length,85);
// All four fingers remain open throughout the gentle breathing/stroking loop.
for(const frame of bake.frames){
    const palm=frame.bones.find(b=>b.name==='palm').matrix;
    for(const finger of ['index','middle','ring','little'])for(const joint of ['base','middle','tip']){
        const bone=frame.bones.find(b=>b.name===finger+'_'+joint).matrix;
        const alignment=palm[4]*bone[4]+palm[5]*bone[5]+palm[6]*bone[6];
        assert(alignment>.94,`${finger}_${joint} curled at ${frame.time}`);
    }
    for(const joint of ['base','middle','tip']){
        const thumb=frame.bones.find(b=>b.name==='thumb_'+joint).matrix;
        assert(palm[4]*thumb[4]+palm[5]*thumb[5]+palm[6]*thumb[6]>.55,'Thumb must stay extended and spread');
    }
}
for(let i=0;i<bake.frames[0].bones.length;i++)for(let j=0;j<16;j++)
    assert(Math.abs(bake.frames[0].bones[i].matrix[j]-bake.frames[84].bones[i].matrix[j])<1e-7,'Idle seam');
console.log(`Hand source: ${model.elements.length} cubes, ${groups.size} joints, eight seamless Idle poses, pixel texture and FBX assets PASS`);
