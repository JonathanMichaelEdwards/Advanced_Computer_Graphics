function main() {
  const { mat4, mat3, vec3 } = glMatrix; 
  const canvas = document.getElementById("glCanvas");
  const gl = canvas.getContext("webgl2");
  if(!gl) {
	  alert("Error enabling WebGL context!");
	  return;
  } 
  canvas.width = canvas.clientWidth;
  canvas.height = canvas.clientHeight;
  gl.viewport(0, 0, canvas.width, canvas.height);
  gl.clearColor(0, 1, 1, 1);
  gl.clear(gl.COLOR_BUFFER_BIT);
  console.log("Wid, Hgt = " + canvas.width + " " + canvas.height);
}

main();


