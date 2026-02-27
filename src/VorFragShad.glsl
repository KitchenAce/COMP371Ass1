#version 330 core

vec3 kda=vec3(1.0,0.5,0.5);
vec3 kdb=vec3(0.5,1.0,0.5);
vec3 kdc=vec3(0.5,0.5,1.0);

float e=5.0;

uniform vec3 lPos;
uniform vec3 lcol;
uniform vec3 objcol;

flat in vec3 mvA;
flat in vec3 mvB;
flat in vec3 mvC;
in vec3 posF;
in vec3 normalF;

out vec4 col;

void main(){
    float la=distance(mvA,posF);
    float lb=distance(mvB,posF);
    float lc=distance(mvC,posF);
    vec3 kd;
    vec3 ka=vec3(0.1,0.05,0.05);
    vec3 ks=vec3(0.3,0.3,0.3);

    if(la<=lb){
        if(la<=lc){
            kd=kda;
        }else{
            kd=kdc;
        }
    }else{
        if(lc<=lb){
            kd=kdc;
        }else{
            kd=kdb;
        }
    }

    //diffuse calculation
    vec3 uNorm= normalize(normalF);
    vec3 ldir=normalize(lPos-posF);
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
