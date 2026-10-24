// Native Blockbench authoring over loopback CDP. Node >= 18, no npm dependencies.
// Launch a dedicated Blockbench --userData <scratch-profile> --remote-debugging-port=9242.
import fs from 'node:fs';
import path from 'node:path';
import net from 'node:net';
import crypto from 'node:crypto';
import {fileURLToPath} from 'node:url';
const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const out = path.join(root, 'gbResources/rhythm_party_liner');
const scratch = path.join(root, 'gbDemo/output/rhythm-check/party-liner');
fs.mkdirSync(out, {recursive:true}); fs.mkdirSync(scratch, {recursive:true});
const pages = await (await fetch('http://127.0.0.1:9242/json', {signal:AbortSignal.timeout(6000)})).json();
const page = pages.find(p => p.type === 'page' && p.url.includes('index.html'));
if (!page) throw Error('Dedicated Blockbench page not found');
const url = new URL(page.webSocketDebuggerUrl);
const socket = net.connect(+url.port, url.hostname);
let buffer = Buffer.alloc(0), upgraded = false, nextId = 0;
const pending = new Map();
const ready = new Promise((resolve,reject) => {
  socket.once('error', reject);
  socket.once('connect', () => socket.write(`GET ${url.pathname} HTTP/1.1\r\nHost: ${url.host}\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Key: ${crypto.randomBytes(16).toString('base64')}\r\nSec-WebSocket-Version: 13\r\n\r\n`));
  let fragments = [];
  socket.on('data', chunk => {
    buffer = Buffer.concat([buffer,chunk]);
    if (!upgraded) {
      const end = buffer.indexOf('\r\n\r\n'); if (end < 0) return;
      if (!/^HTTP\/1\.1 101\b/.test(buffer.subarray(0,end).toString())) return reject(Error('WebSocket upgrade failed: '+buffer.subarray(0,end).toString()));
      buffer = buffer.subarray(end+4); upgraded = true; resolve();
    }
    while (buffer.length >= 2) {
      const first = buffer[0], second = buffer[1]; let n = second & 127, offset = 2;
      if (n === 126) {if(buffer.length < 4) return; n=buffer.readUInt16BE(2); offset=4;}
      if (n === 127) {if(buffer.length < 10) return; n=Number(buffer.readBigUInt64BE(2)); offset=10;}
      if (second & 128) throw Error('Unexpected masked server frame');
      if (buffer.length < offset+n) return;
      const payload=buffer.subarray(offset,offset+n); buffer=buffer.subarray(offset+n);
      if ((first & 15) === 8) {socket.end(); return;}
      if ((first & 15) === 9) {send(payload,10); continue;}
      fragments.push(payload);
      if (first & 128) {
        const message=JSON.parse(Buffer.concat(fragments).toString()); fragments=[];
        const job=pending.get(message.id);
        if(job){pending.delete(message.id); message.error ? job.reject(Error(JSON.stringify(message.error))) : job.resolve(message.result);}
      }
    }
  });
});
function send(payload, opcode=1) {
  const n=payload.length, header=Buffer.alloc(n<126?2:n<65536?4:10);
  header[0]=128|opcode; header[1]=128|(n<126?n:n<65536?126:127);
  if(n>=126 && n<65536)header.writeUInt16BE(n,2); else if(n>=65536)header.writeBigUInt64BE(BigInt(n),2);
  const mask=crypto.randomBytes(4), body=Buffer.from(payload);
  for(let i=0;i<n;i++)body[i]^=mask[i%4];
  socket.write(Buffer.concat([header,mask,body]));
}
const deadline=setTimeout(()=>{console.error('Blockbench CDP timeout');process.exit(1);},90000);
await ready;
async function evaluate(expression) {
  const id=++nextId;
  const result=await new Promise((resolve,reject)=>{pending.set(id,{resolve,reject});send(Buffer.from(JSON.stringify({id,method:'Runtime.evaluate',params:{expression,awaitPromise:true,returnByValue:true}})));});
  if(result.exceptionDetails)throw Error(JSON.stringify(result.exceptionDetails));
  return result.result.value;
}
try {
  const command=process.argv[2] || 'author';
  if(command==='author') {
    const result=await evaluate(fs.readFileSync(path.join(root,'gbScripts/author-party-liner.js'),'utf8'));
    if(!result?.model || !result.texture)throw Error('Native authoring returned no model/texture');
    fs.writeFileSync(path.join(root,'gbResources/rhythm_party_liner.bbmodel'), typeof result.model==='string'?result.model:JSON.stringify(result.model,null,2));
    fs.writeFileSync(path.join(out,'palette.png'),Buffer.from(result.texture,'base64'));
    fs.writeFileSync(path.join(out,'authoring-report.json'),JSON.stringify(result.report,null,2)+'\n');
    console.log(JSON.stringify(result.report,null,2));
  } else if(command==='reload') {
    const source=fs.readFileSync(path.join(root,'gbResources/rhythm_party_liner.bbmodel'),'utf8');
    const result=await evaluate(`(async()=>{const source=${source}; await Codecs.project.load(source,{path:'C:/Hobby/game-box/gbResources/rhythm_party_liner.bbmodel'}); await new Promise((resolve,reject)=>{let count=0;const poll=()=>Texture.all.every(t=>t.width===128 && t.height===128) ? resolve() : ++count>250 ? reject(Error('Reloaded texture did not load')) : setTimeout(poll,20);poll();}); Canvas.updateAll(); return {version:Blockbench.version,cubes:Cube.all.length,groups:Group.all.map(g=>g.name),textures:Texture.all.map(t=>({name:t.name,width:t.width,height:t.height,error:t.error})),format:Format.id};})()`);
    fs.writeFileSync(path.join(out,'reload-report.json'),JSON.stringify(result,null,2)+'\n');console.log(JSON.stringify(result,null,2));
  } else if(command==='viewport') {
    const data=await evaluate(`(()=>{Modes.options.edit.select(); const p=Preview.selected; p.setProjectionMode(true); p.width=1600;p.height=1000;p.renderer.setSize(1600,1000);p.camOrtho.left=-125;p.camOrtho.right=125;p.camOrtho.top=78.125;p.camOrtho.bottom=-78.125;p.camOrtho.zoom=1;p.camOrtho.position.set(160,125,190);p.controls.target.set(0,8,-49);p.camOrtho.updateProjectionMatrix();p.controls.update();Canvas.updateAll();p.render();Project.saved=true;return p.canvas.toDataURL('image/png').split(',')[1];})()`);
    fs.writeFileSync(path.join(out,'blockbench-view.png'),Buffer.from(data,'base64'));
    console.log('Saved actual native Blockbench viewport');
  } else {
    console.log(JSON.stringify(await evaluate(fs.readFileSync(path.resolve(command),'utf8')),null,2));
  }
} finally {clearTimeout(deadline);socket.end();}
