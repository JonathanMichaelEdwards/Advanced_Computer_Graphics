//====GLOBALS=====
var gl;
var mat4 = glMatrix.mat4;
var vec3 = glMatrix.vec3;
var numElems;
var vao;
var projMatrix, viewMatrix;
var mvpMatrixLoc, norMatrixLoc, colLoc;

var mcol = {r:80, g:80, b:128};      //Material color
var bcol = {r:153, g:255, b:255};    //Background color
const canvas = document.getElementById("glCanvas");  //Also used by trackball

var vsSource = `#version 300 es
	precision mediump float;
	in vec3 position;
	in vec3 normal;
	uniform mat4 mvpMatrix;
	uniform mat4 norMatrix;
	out float diffuse;

	const vec3 lightDirection = normalize(vec3(1.0, 1.0, 1.0));
	void main() {
	 vec3 norm = (norMatrix * vec4(normal, 0) ).xyz;
	 norm = normalize(norm);
	 diffuse = max(0.0, dot(norm, lightDirection));
	 gl_Position = mvpMatrix * vec4(position, 1);
	}		
 `;
 
var fsSource = `#version 300 es
	precision mediump float;
	uniform vec3 matColor;
	in float diffuse;
	out vec4 fragColor;
	
	void main() {
	   vec3 col = diffuse * matColor/255.0;
	   fragColor = vec4(col, 1);
	}
 `;

function createShader(gl, type, source) {
  var shader = gl.createShader(type);
  gl.shaderSource(shader, source);
  gl.compileShader(shader);
  var success = gl.getShaderParameter(shader, gl.COMPILE_STATUS);
  if (success) {
    return shader;
  }
  console.log(gl.getShaderInfoLog(shader));
  gl.deleteShader(shader);
}

function createProgram(gl, vertexShader, fragmentShader) {
  var program = gl.createProgram();
  gl.attachShader(program, vertexShader);
  gl.attachShader(program, fragmentShader);
  gl.linkProgram(program);
  var success = gl.getProgramParameter(program, gl.LINK_STATUS);
  if (success) {
    return program;
  } 
  console.log(gl.getProgramInfoLog(program));
  gl.deleteProgram(program);
}

function main() {
	
  gl = canvas.getContext("webgl2");

  if(!gl) {
	  alert("This browser doesn not support WebGL2!");
	  return;
  } 
  canvas.width = canvas.clientWidth;
  canvas.height = canvas.clientHeight;
  gl.viewport(0, 0, canvas.width, canvas.height);
  gl.enable(gl.DEPTH_TEST);
  
  var vertexShader = createShader(gl, gl.VERTEX_SHADER, vsSource);
  var fragmentShader = createShader(gl, gl.FRAGMENT_SHADER, fsSource);

  var program = createProgram(gl, vertexShader, fragmentShader) 
  gl.useProgram(program);  
  
  var obj = loadObjTxt(dolphinData);                        
  var normals = computeNormals(obj.vertices, obj.faces);   //normals in OBJ file are ignored
  numElems = obj.faces.length;
  var numVerts =  obj.vertices.length/3;
  
  //console.log("Number of verts = "+numVerts);
  //console.log("Number of faces = "+numElems/3); 
  
  vao = gl.createVertexArray();
  gl.bindVertexArray(vao);
  
  const positionBuffer = gl.createBuffer();
  gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);
  gl.bufferData(gl.ARRAY_BUFFER, obj.vertices.length*4, gl.STATIC_DRAW);  //4 bytes per data
  gl.bufferSubData(gl.ARRAY_BUFFER, 0, new Float32Array(obj.vertices));
  const positionLoc = gl.getAttribLocation(program, 'position');
  gl.enableVertexAttribArray(positionLoc); 
  gl.vertexAttribPointer(positionLoc, 3, gl.FLOAT, false, 0, 0);  
  
  const normalBuffer = gl.createBuffer();
  gl.bindBuffer(gl.ARRAY_BUFFER, normalBuffer);
  gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(normals), gl.STATIC_DRAW);  //Note: Subdata not required here! 
  const normalLoc = gl.getAttribLocation(program, 'normal');
  gl.enableVertexAttribArray(normalLoc);
  gl.vertexAttribPointer(normalLoc, 3, gl.FLOAT, false, 0, 0);  
 
  const indexBuffer = gl.createBuffer();
  gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, indexBuffer);
  gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(obj.faces), gl.STATIC_DRAW);

  gl.bindVertexArray(null);
  
  viewMatrix = mat4.create();
  mat4.lookAt(viewMatrix, [0, 5, 20], [0,0,0], [0,1,0]);  
  projMatrix = mat4.create();
  mat4.perspective(projMatrix, 40 * Math.PI/180, 1, 10, 100);   
     
  mvpMatrixLoc = gl.getUniformLocation(program, "mvpMatrix");
  norMatrixLoc = gl.getUniformLocation(program, "norMatrix");
  colLoc = gl.getUniformLocation(program, "matColor")
  
  display();
}


function display() {  

  gl.clearColor(bcol.r/255.0, bcol.g/255.0, bcol.b/255.0, 1.0);

  const modelMatrix = mat4.create();
  mat4.rotateY(modelMatrix, modelMatrix, -0.5 * Math.PI);  //-90 degs about y
  
  const mvMatrix = mat4.create(); 
  mat4.multiply(mvMatrix, viewMatrix, modelMatrix);
  const mvpMatrix = mat4.create(); 
  mat4.multiply(mvpMatrix, projMatrix, mvMatrix);
  
  const norMatrix = mat4.create();
  mat4.invert(norMatrix, mvMatrix); 
  
  gl.uniformMatrix4fv(mvpMatrixLoc, false, mvpMatrix); 
  gl.uniformMatrix4fv(norMatrixLoc, true, norMatrix);
  gl.uniform3f(colLoc, mcol.r, mcol.g, mcol.b);  

  gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);   
  gl.bindVertexArray(vao);
  gl.drawElements(gl.TRIANGLES, numElems, gl.UNSIGNED_SHORT, 0);
  
  gl.bindVertexArray(null);
  gl.bindBuffer(gl.ARRAY_BUFFER, null);
}


main();