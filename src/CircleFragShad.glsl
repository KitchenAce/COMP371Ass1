#version 330 core

vec3 kdo=vec3(1.0,0.5,0.5);
vec3 kao=vec3(0.1,0.05,0.05);
vec3 kso=vec3(0.3,0.3,0.3);

vec3 kdi=vec3(0.5,0.5,1.0);
vec3 kai=vec3(0.05,0.05,0.1);
vec3 ksi=vec3(0.0,0.0,0.0);
float e=5.0;

uniform vec3 lPos;
uniform vec3 lcol;
uniform vec3 objcol;

in vec3 mvA;
in vec3 mvB;
in vec3 mvC;
in vec3 posF;
in vec3 normalF;

out vec4 col;

void main(){
    //calculation for center and radius
    float a = length(mvB-mvC);
    float b = length(mvC-mvA);
    float c = length(mvA-mvB);
    vec3 ic = (a*mvA+b*mvB+c*mvC)/(a+b+c);
    float area = 0.5*length(cross(mvB-mvA,mvC-mvA));
    float s = (a+b+c)*0.5;
    float r = area/s;
    //calculating to see if point is in the circle or not
    float l = length(posF-ic);
    vec3 kd=kdi;
    vec3 ka=kai;
    vec3 ks=ksi;

    if(l<=r){
        kd=kdo;
        ka=kao;
        ks=kso;
    }

    //diffuse calculation
    vec3 uNorm= normalize(normalF);
    vec3 ldir=normalize(lPos-posF);
    float ldist=length(lPos-posF);
    float costh=dot(uNorm,ldir);
    float E=max(0.0,costh);
    vec3 Lrd=kd*lcol*E;
    //specular calcuation
    vec3 v=normalize(-posF);
    vec3 h=normalize(ldir+v);
    float res=pow(max(0.00,dot(uNorm,h)),e);
    vec3 Lrs=(ks*res)*lcol*E;
    //ambiant calcuation
    vec3 Lra=ka*lcol;
    //final calculation
    vec3 Lr=(Lrs+Lrd+Lra)*objcol;
    col=vec4(Lr,1.0);
}
