// Node >=22 (also available via ELECTRON_RUN_AS_NODE=1 Blockbench.exe).
import fs from 'node:fs';
const pages = await (await fetch('http://127.0.0.1:9238/json')).json();
const page = pages.find(p=>p.type==='page');
const socket = new WebSocket(page.webSocketDebuggerUrl);
const timeout = setTimeout(()=>{console.error('Blockbench evaluation timed out');process.exit(1);},30000);
await new Promise((resolve,reject)=>{socket.onopen=resolve;socket.onerror=reject;});
socket.onmessage = e=>{const r=JSON.parse(e.data);if(r.id===1){
 const value=r.result?.result?.value;
 if(value?.model){fs.writeFileSync('gbResources/hand_cursor.bbmodel',typeof value.model==='string'?value.model:JSON.stringify(value.model,null,2));fs.writeFileSync('gbBundle/Textures/hand_cursor.png',Buffer.from(value.texture,'base64'));fs.writeFileSync('gbResources/hand_cursor.animation.json',JSON.stringify(value.bake));fs.writeFileSync('gbDemo/output/hand-cursor/authoring.json',JSON.stringify(value.report,null,2));console.log(value.report);}
 else if(value?.bake){fs.writeFileSync('gbResources/hand_cursor.animation.json',JSON.stringify(value.bake));console.log(`Native Blockbench animation baked: ${value.bake.frames.length} frames`);}
 else console.log(JSON.stringify(r,null,2));
 clearTimeout(timeout);socket.close();if(r.result?.exceptionDetails)process.exitCode=1;}};
const input=process.argv[2]||(fs.existsSync('gbResources/hand_cursor.bbmodel')?'gbScripts/animate-hand-cursor.js':'gbScripts/author-hand-cursor.js');
const prelude=input.includes('animate-hand-cursor')?`globalThis.handCursorSource=${fs.readFileSync('gbResources/hand_cursor.bbmodel','utf8')};\n`:'';
socket.send(JSON.stringify({id:1,method:'Runtime.evaluate',params:{expression:prelude+fs.readFileSync(input,'utf8'),awaitPromise:true,returnByValue:true}}));
