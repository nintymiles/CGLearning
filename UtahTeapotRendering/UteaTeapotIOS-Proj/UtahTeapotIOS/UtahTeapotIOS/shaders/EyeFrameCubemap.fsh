#version 300 es

precision highp float;

uniform lowp vec3       vMaterialAmbient;
uniform lowp vec4       vMaterialSpecular;

uniform highp vec3   vLight0;

uniform samplerCube samplerObj;

in lowp vec4 colorDiffuse;

in mediump vec3 position;
in mediump vec3 normal;

out mediump vec4 fragColor;

void main()
{
    mediump vec3 halfVector = normalize(vLight0 - position);
    mediump float NdotH = max(dot(normalize(normal), halfVector), 0.0);
    mediump float fPower = vMaterialSpecular.w;
    mediump float specular = pow(NdotH, fPower);
    
    vec3 reflected = reflect(normalize(position),normalize(normal));

    lowp vec4 colorSpecular = vec4( vMaterialSpecular.xyz * specular, 1 );
    vec3 texCoord = reflected;
    //fragColor = texture(samplerObj, normalize(texCoord));
    fragColor =  texture(samplerObj, texCoord) ;//+ colorSpecular* vMaterialSpecular + vec4(vMaterialAmbient.xyz, 1.0);
}
