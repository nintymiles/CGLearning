uniform float uVertexScale;
uniform sampler2D uTexUnit0, uTexUnit1;

varying vec2 vTexCoord0, vTexCoord1;
varying vec3 vColor;

void main(void) {
  vec4 color = vec4(vColor.x, vColor.y, vColor.z, 1);
  vec4 texColor0 = texture2D(uTexUnit0, vTexCoord0);
  vec4 texColor1 = texture2D(uTexUnit1, vTexCoord1);

  float lerper = clamp(.5 *uVertexScale, 0., 1.);
  float lerper2 = clamp(.5 * uVertexScale + 1.0, 0.0, 1.0);

  gl_FragColor = ((lerper)*texColor1 + (1.0-lerper)*texColor0) * lerper2 + color * (1.0-lerper2);
}
