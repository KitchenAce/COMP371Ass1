#version 330 core

in vec3 PosF;

uniform vec3 objcol;
uniform vec3 lPos;
uniform vec3 lcol;
uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float e;

out vec4 col;

//diffuse (1,0.5,0.5
//ambient 0.1 0.05 0.05
//spec 0.3, 0.3, 0.3 and exponent 5
// light 1,1,1

void main(){
    /*face normal calculation using derivation of the
    faces position, producing two directions that are
    parallel to the face surface. The cross product
    of the two is the normal of the face*/
    vec3 xDir= dFdx(PosF);
    vec3 yDir=dFdy(PosF);
    vec3 fNorm=normalize(cross(xDir,yDir));
    //diffuse calculation
    vec3 uNorm= normalize(fNorm);
    vec3 ldir=normalize(lPos-PosF);
    float costh=dot(uNorm,ldir);
    float E=max(0.0,costh);
    vec3 Lrd=kd*lcol*E;
    //specular calcuation
    vec3 v=normalize(-PosF);
    vec3 h=normalize(ldir+v);
    float res=pow(max(0.00,dot(uNorm,h)),e);
    vec3 Lrs=(ks*res)*lcol*E;
    //ambiant calcuation
    vec3 Lra=ka*lcol;
    //final calculation
    vec3 Lr=(Lrs+Lrd+Lra)*objcol;
    col=vec4(Lr,1.0);
}
