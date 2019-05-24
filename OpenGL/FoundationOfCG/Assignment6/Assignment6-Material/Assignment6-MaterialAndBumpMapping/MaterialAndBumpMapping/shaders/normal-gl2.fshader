uniform sampler2D uTexColor;
uniform sampler2D uTexNormal;

// lights in eye space
uniform vec3 uLight;
uniform vec3 uLight2;

varying vec2 vTexCoord;
varying mat3 vNTMat;
varying vec3 vEyePos;

void main() {
  // TODO: replace the following line with loading of normal from uTexNormal
  //       transforming to eye space, and normalizing
  vec3 normal = vec3(0, 0, 1);

  vec3 viewDir = normalize(-vEyePos);
  vec3 lightDir = normalize(uLight - vEyePos);
  vec3 lightDir2 = normalize(uLight2 - vEyePos);

  float nDotL = dot(normal, lightDir);
  vec3 reflection = normalize( 2.0 * normal * nDotL - lightDir);
  float rDotV = max(0.0, dot(reflection, viewDir));
  float specular = pow(rDotV, 32.0);
  float diffuse = max(nDotL, 0.0);

  nDotL = dot(normal, lightDir2);
  reflection = normalize( 2.0 * normal * nDotL - lightDir2);
  rDotV = max(0.0, dot(reflection, viewDir));
  specular += pow(rDotV, 32.0);
  diffuse += max(nDotL, 0.0);

  vec3 color = texture2D(uTexColor, vTexCoord).xyz * diffuse + specular * vec3(0.6, 0.6, 0.6);
  gl_FragColor = vec4(color, 1);
}
