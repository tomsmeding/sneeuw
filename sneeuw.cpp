#include <iostream>
#include <unordered_map>
#include <list>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <unistd.h>

// #define FLAKESTRING "❄️"
#define FLAKESTRING "*"

using namespace std;

unordered_map<string,string> tputcache;

string gettput(string cap){
	auto it=tputcache.find(cap);
	if(it!=tputcache.end())return it->second;
	char *cmd;
	asprintf(&cmd,"/usr/bin/tput %s",cap.data());
	cout.flush(); //flush for popen
	FILE *proc=popen(cmd,"r");
	free(cmd);
	if(proc==NULL)return string();
	char buf[64]; //should be enough
	size_t nread=fread(buf,1,64,proc);
	pclose(proc);
	string res(buf,nread);
	tputcache[cap]=res;
	return res;
}

double randomf(double limit){
	return (double)rand()/RAND_MAX*limit;
}

struct Particle{
	double x,y;
	double angle,speed;

	Particle(double x,double y,double angle,double speed):x(x),y(y),angle(angle),speed(speed){}
};

class State{
public:
	const int W,H;
	bool *bd;
	string bdstring;
	list<Particle> part;

	State(int W,int H):W(W),H(H){
		bd=new bool[W*H];
	}
	~State(void){
		if(bd)delete[] bd;
	}

	void genpart(int number){
		while(number-->0){
			part.emplace_back(randomf(W),randomf(H/2)-H/2,randomf(M_PI/3)+M_PI/3,randomf(0.5)+0.6);
		}
	}
	void iteration(int geniniter=0){
		if(geniniter>0)genpart(geniniter);
		double dx,l;
		for(Particle &p : part){
			//cerr<<"$";
			//Particle &p=*it;
			//cerr<<&p;
			dx=cos(p.angle);
			l=sqrt(dx*dx+1);
			p.x+=dx/l*p.speed;
			p.y+=p.speed/l;
			p.x+=randomf(0.3)-0.15;
			p.y+=randomf(0.3)-0.15;
			p.angle+=randomf(M_PI/10)-M_PI/20;
		}
		//cerr<<"***"<<endl;
		double HH=H;
		part.remove_if([HH](const Particle &p){
			return p.y>HH;
		});
	}
	void render(void){
		memset(bd,0,W*H*sizeof(bool));
		int x,y;
		for(const Particle &pos : part){
			y=pos.y+.5;
			// if(y<0||y>=H)continue;
			if(pos.y<-.3||y>=H)continue; //seems to look better
			x=((int)(pos.x+.5)%W+W)%W;
			bd[W*y+x]=true;
		}
		bdstring.clear();
		bdstring.reserve(W*H);
		for(x=0;x<W*H;x++){
			if(bd[x])bdstring+=FLAKESTRING;
			else bdstring+=' ';
		}
	}
	/*void border(void){
		int x,y;
		y=0;
		for(x=0;x<W-1;x++)bd[W*y+x]='#';
		for(;y<H-1;y++)bd[W*y+x]='#';
		for(;x>0;x--)bd[W*y+x]='#';
		for(;y>0;y--)bd[W*y+x]='#';
	}*/
	void print(void){
		cout<<"\x1B[H\x1B[2J"<<bdstring<<flush;
	}
};

int main(int argc,char **argv){
	int W,H;
	/*if(argc==3){
		W=strtol(argv[1],NULL,10);
		H=strtol(argv[2],NULL,10);
	} else {*/
		W=strtol(gettput("cols").data(),NULL,10);
		H=strtol(gettput("lines").data(),NULL,10);
	/*}*/
	bool infinite=false;
	if(argc==2&&strcmp(argv[1],"inf")==0){
		infinite=true;
	}
	State state(W,H);
	if(!infinite)state.genpart(W*H/20);
	else state.genpart(W*H/60);
	state.render();
	state.print();
	while(state.part.size()){
		state.iteration(infinite*W*H/1100);
		state.render();
		//state.border();
		state.print();
		usleep(100000);
	}
}
