clear all
clear all
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% This code is to calculate the packet delivery ratio, packet delay, and
% enenrgy consumption over time for X-MAC without retransmissions in a
% fully-connected network using the Markov model.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

CCA=1; % CCA=0.5ms
T=1000; % T=500ms
ADV=3;  % ADV=1.5ms
ACK=1;  % ACK=0.5ms
DATA=5; % DATA=2.5ms
active=30; %active=15ms;
lambda=1.5/1000;   % 1.5pkt/1000ms
lambda=200/1;   %avg=200ms interval a pkt

sim=50000;
%lambda=1/1000;%5/1000; %5pkt/s

ADV=3;
ACK=1;
DATA=5;
active=15;
N=40;%5;
Q=10;
R=0;
resolution=0.01;
index=101;
% 1 slot = 1 ms

T=200;%100;
lamda=1/1000; %1 pkt / 200 slot
W=128;

figure(1)


ptx_rec1=zeros(1,index);
X1_rec1=zeros(1,index);

ptx_rec2=zeros(1,index);
X1_rec2=zeros(1,index);
nbusy_rec2=zeros(1,index);
psuc=zeros(1,index);

b=-0.1;
for l=1:1
    b=b+0.2;

%    T=m*(1-b)/b+1+m;
    
    
    p0=exp(-lamda*T)*(lamda*T)^0/1;
    p1=exp(-lamda*T)*(lamda*T)^1/1;
    p2=exp(-lamda*T)*(lamda*T)^2/2;
    p3=exp(-lamda*T)*(lamda*T)^3/6;
    p4=exp(-lamda*T)*(lamda*T)^4/24;
    p5=exp(-lamda*T)*(lamda*T)^5/120;
    p6=exp(-lamda*T)*(lamda*T)^6/720;
    p7=exp(-lamda*T)*(lamda*T)^7/(720*7);
    p8=exp(-lamda*T)*(lamda*T)^8/(720*56);
    p9=exp(-lamda*T)*(lamda*T)^9/(720*7*8*9);
    p10=exp(-lamda*T)*(lamda*T)^10/(720*7*8*9*10);
    p11=exp(-lamda*T)*(lamda*T)^11/(720*7*8*9*10*11);
    p12=exp(-lamda*T)*(lamda*T)^12/(720*7*8*9*10*11*12);
    p13=exp(-lamda*T)*(lamda*T)^13/(720*7*8*9*10*11*12*13);
    p14=exp(-lamda*T)*(lamda*T)^14/(720*7*8*9*10*11*12*13*14);
    p15=exp(-lamda*T)*(lamda*T)^15/(720*7*8*9*10*11*12*13*14*15);
    p16=exp(-lamda*T)*(lamda*T)^16/(720*7*8*9*10*11*12*13*14*15*16);
    p17=exp(-lamda*T)*(lamda*T)^17/(720*7*8*9*10*11*12*13*14*15*16*17);
    p18=exp(-lamda*T)*(lamda*T)^18/(720*7*8*9*10*11*12*13*14*15*16*17*18);
    p19=exp(-lamda*T)*(lamda*T)^19/(720*7*8*9*10*11*12*13*14*15*16*17*18*19);
    p20=exp(-lamda*T)*(lamda*T)^20/(720*7*8*9*10*11*12*13*14*15*16*17*18*19*20);
    p21=exp(-lamda*T)*(lamda*T)^21/(720*7*8*9*10*11*12*13*14*15*16*17*18*19*20*21);
    p22=exp(-lamda*T)*(lamda*T)^22/(720*7*8*9*10*11*12*13*14*15*16*17*18*19*20*21*22);
    p23=exp(-lamda*T)*(lamda*T)^23/(720*7*8*9*10*11*12*13*14*15*16*17*18*19*20*21*22*23);
    p24=exp(-lamda*T)*(lamda*T)^24/(720*7*8*9*10*11*12*13*14*15*16*17*18*19*20*21*22*23*24);
    p25=exp(-lamda*T)*(lamda*T)^25/(720*7*8*9*10*11*12*13*14*15*16*17*18*19*20*21*22*23*24*25);
    p26=exp(-lamda*T)*(lamda*T)^26/(720*7*8*9*10*11*12*13*14*15*16*17*18*19*20*21*22*23*24*25*26);
    p27=exp(-lamda*T)*(lamda*T)^27/(720*7*8*9*10*11*12*13*14*15*16*17*18*19*20*21*22*23*24*25*26*27);
    p28=exp(-lamda*T)*(lamda*T)^28/(720*7*8*9*10*11*12*13*14*15*16*17*18*19*20*21*22*23*24*25*26*27*28);
    p29=exp(-lamda*T)*(lamda*T)^29/(720*7*8*9*10*11*12*13*14*15*16*17*18*19*20*21*22*23*24*25*26*27*28*29);
    p30=exp(-lamda*T)*(lamda*T)^30/(720*7*8*9*10*11*12*13*14*15*16*17*18*19*20*21*22*23*24*25*26*27*28*29*30);
    p31=exp(-lamda*T)*(lamda*T)^31/(720*7*8*9*10*11*12*13*14*15*16*17*18*19*20*21*22*23*24*25*26*27*28*29*30*31);
    p32=exp(-lamda*T)*(lamda*T)^32/(720*7*8*9*10*11*12*13*14*15*16*17*18*19*20*21*22*23*24*25*26*27*28*29*30*31*32);
    p33=exp(-lamda*T)*(lamda*T)^33/(720*7*8*9*10*11*12*13*14*15*16*17*18*19*20*21*22*23*24*25*26*27*28*29*30*31*32*33);
    p34=exp(-lamda*T)*(lamda*T)^34/(720*7*8*9*10*11*12*13*14*15*16*17*18*19*20*21*22*23*24*25*26*27*28*29*30*31*32*33*34);
    p35=exp(-lamda*T)*(lamda*T)^35/(720*7*8*9*10*11*12*13*14*15*16*17*18*19*20*21*22*23*24*25*26*27*28*29*30*31*32*33*34*35);
    p36=exp(-lamda*T)*(lamda*T)^36/(720*7*8*9*10*11*12*13*14*15*16*17*18*19*20*21*22*23*24*25*26*27*28*29*30*31*32*33*34*35*36);
    p37=exp(-lamda*T)*(lamda*T)^37/(720*7*8*9*10*11*12*13*14*15*16*17*18*19*20*21*22*23*24*25*26*27*28*29*30*31*32*33*34*35*36*37);
    p38=exp(-lamda*T)*(lamda*T)^38/(720*7*8*9*10*11*12*13*14*15*16*17*18*19*20*21*22*23*24*25*26*27*28*29*30*31*32*33*34*35*36*37*38);
    p39=exp(-lamda*T)*(lamda*T)^39/(720*7*8*9*10*11*12*13*14*15*16*17*18*19*20*21*22*23*24*25*26*27*28*29*30*31*32*33*34*35*36*37*38*39);
    p40=exp(-lamda*T)*(lamda*T)^40/(720*7*8*9*10*11*12*13*14*15*16*17*18*19*20*21*22*23*24*25*26*27*28*29*30*31*32*33*34*35*36*37*38*39*40);
    p41=exp(-lamda*T)*(lamda*T)^41/(720*7*8*9*10*11*12*13*14*15*16*17*18*19*20*21*22*23*24*25*26*27*28*29*30*31*32*33*34*35*36*37*38*39*40*41);
    p42=exp(-lamda*T)*(lamda*T)^42/(720*7*8*9*10*11*12*13*14*15*16*17*18*19*20*21*22*23*24*25*26*27*28*29*30*31*32*33*34*35*36*37*38*39*40*41*42);
    p43=exp(-lamda*T)*(lamda*T)^43/(720*7*8*9*10*11*12*13*14*15*16*17*18*19*20*21*22*23*24*25*26*27*28*29*30*31*32*33*34*35*36*37*38*39*40*41*42*43);
    p44=exp(-lamda*T)*(lamda*T)^44/(720*7*8*9*10*11*12*13*14*15*16*17*18*19*20*21*22*23*24*25*26*27*28*29*30*31*32*33*34*35*36*37*38*39*40*41*42*43*44);
    p45=exp(-lamda*T)*(lamda*T)^45/(720*7*8*9*10*11*12*13*14*15*16*17*18*19*20*21*22*23*24*25*26*27*28*29*30*31*32*33*34*35*36*37*38*39*40*41*42*43*44*45);
    p46=exp(-lamda*T)*(lamda*T)^46/(720*7*8*9*10*11*12*13*14*15*16*17*18*19*20*21*22*23*24*25*26*27*28*29*30*31*32*33*34*35*36*37*38*39*40*41*42*43*44*45*46);
    p47=exp(-lamda*T)*(lamda*T)^47/(720*7*8*9*10*11*12*13*14*15*16*17*18*19*20*21*22*23*24*25*26*27*28*29*30*31*32*33*34*35*36*37*38*39*40*41*42*43*44*45*46*47);
    p48=exp(-lamda*T)*(lamda*T)^48/(720*7*8*9*10*11*12*13*14*15*16*17*18*19*20*21*22*23*24*25*26*27*28*29*30*31*32*33*34*35*36*37*38*39*40*41*42*43*44*45*46*47*48);
    p49=exp(-lamda*T)*(lamda*T)^49/(720*7*8*9*10*11*12*13*14*15*16*17*18*19*20*21*22*23*24*25*26*27*28*29*30*31*32*33*34*35*36*37*38*39*40*41*42*43*44*45*46*47*48*49);
    p50=exp(-lamda*T)*(lamda*T)^50/(720*7*8*9*10*11*12*13*14*15*16*17*18*19*20*21*22*23*24*25*26*27*28*29*30*31*32*33*34*35*36*37*38*39*40*41*42*43*44*45*46*47*48*49*50);
    
    pp01  =1;
    pp12  =1-p0;
    pp23  =1-p0-p1;
    pp34  =1-p0-p1-p2;
    pp45  =1-p0-p1-p2-p3;
    pp56  =1-p0-p1-p2-p3-p4;
    pp67  =1-p0-p1-p2-p3-p4-p5;
    pp78  =1-p0-p1-p2-p3-p4-p5-p6;
    pp89  =1-p0-p1-p2-p3-p4-p5-p6-p7;
    pp910 =1-p0-p1-p2-p3-p4-p5-p6-p7-p8;
    pp1011=1-p0-p1-p2-p3-p4-p5-p6-p7-p8-p9;
    pp1112=pp1011-p10;
    pp1213=pp1011-p10-p11;
    pp1314=pp1011-p10-p11-p12;
    pp1415=pp1011-p10-p11-p12-p13;
    pp1516=pp1011-p10-p11-p12-p13-p14;
    pp1617=pp1011-p10-p11-p12-p13-p14-p15;
    pp1718=pp1011-p10-p11-p12-p13-p14-p15-p16;
    pp1819=pp1011-p10-p11-p12-p13-p14-p15-p16-p17;
    pp1920=pp1011-p10-p11-p12-p13-p14-p15-p16-p17-p18;
    pp2021=pp1011-p10-p11-p12-p13-p14-p15-p16-p17-p18-p19;
    pp2122=pp2021-p20;
    pp2223=pp2021-p20-p21;
    pp2324=pp2021-p20-p21-p22;
    pp2425=pp2021-p20-p21-p22-p23;
    pp2526=pp2021-p20-p21-p22-p23-p24;
    pp2627=pp2021-p20-p21-p22-p23-p24-p25;
    pp2728=pp2021-p20-p21-p22-p23-p24-p25-p26;
    pp2829=pp2021-p20-p21-p22-p23-p24-p25-p26-p27;
    pp2930=pp2021-p20-p21-p22-p23-p24-p25-p26-p27-p28;
    pp3031=pp2021-p20-p21-p22-p23-p24-p25-p26-p27-p28-p29;
    pp3132=pp3031-p30;
    pp3233=pp3031-p30-p31;
    pp3334=pp3031-p30-p31-p32;
    pp3435=pp3031-p30-p31-p32-p33;
    pp3536=pp3031-p30-p31-p32-p33-p34;
    pp3637=pp3031-p30-p31-p32-p33-p34-p35;
    pp3738=pp3031-p30-p31-p32-p33-p34-p35-p36;
    pp3839=pp3031-p30-p31-p32-p33-p34-p35-p36-p37;
    pp3940=pp3031-p30-p31-p32-p33-p34-p35-p36-p37-p38;
    pp4041=pp3031-p30-p31-p32-p33-p34-p35-p36-p37-p38-p39;
    pp4142=pp4041-p40;
    pp4243=pp4041-p40-p41;
    pp4344=pp4041-p40-p41-p42;
    pp4445=pp4041-p40-p41-p42-p43;
    pp4546=pp4041-p40-p41-p42-p43-p44;
    pp4647=pp4041-p40-p41-p42-p43-p44-p45;
    pp4748=pp4041-p40-p41-p42-p43-p44-p45-p46;
    pp4849=pp4041-p40-p41-p42-p43-p44-p45-p46-p47;
    pp4950=pp4041-p40-p41-p42-p43-p44-p45-p46-p47-p48;
    pp5051=pp4041-p40-p41-p42-p43-p44-p45-p46-p47-p48-p49;

    num1=0;
    for p=0:resolution:1
        if (Q==10)
            A=zeros(11,11);
            A=[p0 p1 p2 p3 p4 p5 p6 p7 p8 p9 pp1011
                p*p0 p*p1+(1-p)*p0 p*p2+(1-p)*p1 p*p3+(1-p)*p2 p*p4+(1-p)*p3 p*p5+(1-p)*p4 p*p6+(1-p)*p5 p*p7+(1-p)*p6 p*p8+(1-p)*p7 p*p9+(1-p)*p8 p*pp1011+(1-p)*pp910
                0 p*p0 p*p1+(1-p)*p0 p*p2+(1-p)*p1 p*p3+(1-p)*p2 p*p4+(1-p)*p3 p*p5+(1-p)*p4 p*p6+(1-p)*p5 p*p7+(1-p)*p6 p*p8+(1-p)*p7 p*pp910+(1-p)*pp89
                0 0 p*p0 p*p1+(1-p)*p0 p*p2+(1-p)*p1 p*p3+(1-p)*p2 p*p4+(1-p)*p3 p*p5+(1-p)*p4 p*p6+(1-p)*p5 p*p7+(1-p)*p6 p*pp89+(1-p)*pp78
                0 0 0 p*p0 p*p1+(1-p)*p0 p*p2+(1-p)*p1 p*p3+(1-p)*p2 p*p4+(1-p)*p3 p*p5+(1-p)*p4 p*p6+(1-p)*p5 p*pp78+(1-p)*pp67
                0 0 0 0 p*p0 p*p1+(1-p)*p0 p*p2+(1-p)*p1 p*p3+(1-p)*p2 p*p4+(1-p)*p3 p*p5+(1-p)*p4 p*pp67+(1-p)*pp56
                0 0 0 0 0 p*p0 p*p1+(1-p)*p0 p*p2+(1-p)*p1 p*p3+(1-p)*p2 p*p4+(1-p)*p3 p*pp56+(1-p)*pp45
                0 0 0 0 0 0 p*p0 p*p1+(1-p)*p0 p*p2+(1-p)*p1 p*p3+(1-p)*p2 p*pp45+(1-p)*pp34    
                0 0 0 0 0 0 0 p*p0 p*p1+(1-p)*p0 p*p2+(1-p)*p1 p*pp34+(1-p)*pp23    
                0 0 0 0 0 0 0 0 p*p0 p*p1+(1-p)*p0 p*pp23+(1-p)*pp12    
                0 0 0 0 0 0 0 0 0 p*p0 p*pp12+(1-p)*pp01]
            A=A'-eye(11);
            A(11,:)=ones(1,11);
            B=zeros(11,1);
            B(11,1)=1;
        end;
    
        X0=A\B
        v=X0(1);
        num1=num1+1;        
        ptx_rec1(num1)=p;
        X1_rec1(num1)=v;
                
    end;
    plot(ptx_rec1(:),X1_rec1(:),'b');%,axis([0,1,0,1]);
    xlabel('p');
    ylabel('\pi _0');
    title(['N=',num2str(N),' W=',num2str(W),' Q=',num2str(Q),' \lambda=',num2str(lamda*10000),'pkt/10s']);
    hold on
end
hold on

ps_rec2=zeros(1,index);
pf_rec2=zeros(1,index);
X1_rec2=zeros(1,index);
a=0;
b=0;
c=0;
d=0;
e=0;
f=0;

%figure(2)
i=0;
for pp0=0:resolution:1
    i=i+1
    
    
    X1_rec2(i)=pp0;
       
     Enb=0;
     Pnb=0;
     for t=1:T
         for ii=0:N-1
             % i nodes wake up before t, but have nothing to send
             a=nchoosek(N,ii)*((t-1)/T)^ii*pp0^ii;
             % N-i nodes wake up from t, have pkts to send =
             for j=1:N-ii
                 % j nodes wake up at t
                 b=nchoosek(N-ii,j)*(1/T)^j;
                 for k=1:j
                     % k nodes in j nodes have pkt to send
                     c=nchoosek(j,k)*pp0^(j-k)*(1-pp0)^k;
                     % N-i-j nodes wake up from t+1
                     d=((T-t)/T)^(N-ii-j);
                     for n=0:1000
                        e=pp0^(N*n);
                        Pnb=Pnb+a*b*c*d*e;
                        Enb=Enb+a*b*c*d*e*(n*T+t);
                     end;
                 end;
             end;
         end;
         Pnb;
     end;
    Enb=Enb/Pnb
    
     
     Pst=0;
     for t=1:T
         for ii=0:N-1
             % i nodes wake up before t, but have nothing to send
             a=nchoosek(N,ii)*((t-1)/T)^ii*pp0^ii;
             % N-i nodes wake up from t, have pkts to send =
             for j=1:1
                 % j=1 node wakes up at t
                 b=nchoosek(N-ii,j)*(1/T)^j;
                 for k=1:j
                     % k nodes in j=1 nodes have pkt to send
                     c=nchoosek(j,k)*pp0^(j-k)*(1-pp0)^k;                 
                     % N-i-j=N-i-1 nodes wake up from t+1
                     d=((T-t)/T)^(N-ii-j);
                     for n=0:1000
                         e=pp0^(N*n);
                         Pst=Pst+a*b*c*d*e;                        
                     end;
                 end;
             end;
         end;
         Pnb;
     end;
    Pst ;

     Pft=0;
     for t=1:T
         for ii=0:N-2
             % i nodes wake up before t, but have nothing to send
             a=nchoosek(N,ii)*((t-1)/T)^ii*pp0^ii;
             % N-i nodes wake up from t, have pkts to send =
             for j=2:N-ii
                 % j in N-ii nodes wakes up at t
                 b=nchoosek(N-ii,j)*(1/T)^j;
                 for k=2:j
                     % k nodes in j>=2 nodes have pkt to send
                     c=nchoosek(j,k)*pp0^(j-k)*(1-pp0)^k;                     
                     % N-ii-j=N-i-1 nodes wake up from t+1
                     d=((T-t)/T)^(N-ii-j);
                     for n=0:1000
                         e=pp0^(N*n);
                         Pft=Pft+a*b*c*d*e;                        
                     end;
                 end;
             end;
         end;
         Pnb;
     end;
    Pft ;
    
    Et=(Pst*(T/2+DATA)+Pft*T)/(Pst+Pft);
    Busy=Et/(Et+Enb);
    Nbusy=1-Busy;

    %plot([pp0],[Nbusy]);
    %hold on
    
    
    Ps_=0; %Ps_=P(suc tx| nbusy , 1-pi0)
    Pf_=0; %Pf_=P(fail tx| nbusy , 1-pi0)
    

    for t=1:T
        for j=0:N-1
            % i nodes wake up at t
            a=1/T*nchoosek(N-1,j)*(1/T)^j*pp0^j*((T-1)/T)^(N-1-j);
            Ps_=Ps_+a;
            % i node wake up at t
            if (j>0)
                % j nodes wake up at t, with a pkt
                for k=1:j
                    b=1/T*nchoosek(N-1,j)*(1/T)^j*nchoosek(j,k)*pp0^(j-k)*(1-pp0)^k*((T-1)/T)^(N-1-j);
                    Pf_=Pf_+b;
                end;
            end;            
        end;
    end;
            
    
    ps_rec2(i)=Ps_*Nbusy;
    pf_rec2(i)=Pf_*Nbusy;
    nbusy_rec2(i)=Nbusy;
end;
%hold off



plot(ps_rec2(:)+pf_rec2(:),X1_rec2(:),'r')
hold off

minimum=1000;
k=0;
for i=1:index
    for j=1:index
        if ((ptx_rec1(i)-ps_rec2(j)-pf_rec2(j))^2+(X1_rec1(i)-X1_rec2(j))^2<minimum)
            k=j;
            minimum=(ptx_rec1(i)-ps_rec2(j)-pf_rec2(j))^2+(X1_rec1(i)-X1_rec2(j))^2;            
        end;
    end;
end;

ps=ps_rec2(k);
pf=pf_rec2(k);
pp0=X1_rec2(k);
PDR=(1-pp0)*ps/(lamda*T)
%throughput=PDR*(2000*1000*lamda)*N;                      

ps/nbusy_rec2(k)
pf/nbusy_rec2(k)
nbusy_rec2(k)

PDR
ps
pf
pp0
nbusy_rec2(k)



p=ps+pf;
dl=0;
for g=0:10000
    g0=p*(1-p)^g;
    dl=dl+g0*g*T;
end;
dl=dl+T/2+DATA;

if (Q==10)
    A=zeros(11,11);
    A=[p0 p1 p2 p3 p4 p5 p6 p7 p8 p9 pp1011
         p*p0 p*p1+(1-p)*p0 p*p2+(1-p)*p1 p*p3+(1-p)*p2 p*p4+(1-p)*p3 p*p5+(1-p)*p4 p*p6+(1-p)*p5 p*p7+(1-p)*p6 p*p8+(1-p)*p7 p*p9+(1-p)*p8 p*pp1011+(1-p)*pp910
         0 p*p0 p*p1+(1-p)*p0 p*p2+(1-p)*p1 p*p3+(1-p)*p2 p*p4+(1-p)*p3 p*p5+(1-p)*p4 p*p6+(1-p)*p5 p*p7+(1-p)*p6 p*p8+(1-p)*p7 p*pp910+(1-p)*pp89
         0 0 p*p0 p*p1+(1-p)*p0 p*p2+(1-p)*p1 p*p3+(1-p)*p2 p*p4+(1-p)*p3 p*p5+(1-p)*p4 p*p6+(1-p)*p5 p*p7+(1-p)*p6 p*pp89+(1-p)*pp78
         0 0 0 p*p0 p*p1+(1-p)*p0 p*p2+(1-p)*p1 p*p3+(1-p)*p2 p*p4+(1-p)*p3 p*p5+(1-p)*p4 p*p6+(1-p)*p5 p*pp78+(1-p)*pp67
         0 0 0 0 p*p0 p*p1+(1-p)*p0 p*p2+(1-p)*p1 p*p3+(1-p)*p2 p*p4+(1-p)*p3 p*p5+(1-p)*p4 p*pp67+(1-p)*pp56
         0 0 0 0 0 p*p0 p*p1+(1-p)*p0 p*p2+(1-p)*p1 p*p3+(1-p)*p2 p*p4+(1-p)*p3 p*pp56+(1-p)*pp45
         0 0 0 0 0 0 p*p0 p*p1+(1-p)*p0 p*p2+(1-p)*p1 p*p3+(1-p)*p2 p*pp45+(1-p)*pp34    
         0 0 0 0 0 0 0 p*p0 p*p1+(1-p)*p0 p*p2+(1-p)*p1 p*pp34+(1-p)*pp23    
         0 0 0 0 0 0 0 0 p*p0 p*p1+(1-p)*p0 p*pp23+(1-p)*pp12    
         0 0 0 0 0 0 0 0 0 p*p0 p*pp12+(1-p)*pp01]
     A=A'-eye(11);
     A(11,:)=ones(1,11);
     B=zeros(11,1);
     B(11,1)=1;
end;
X0=A\B
        
delay=0;
expect=0;
for f=1:9
    expect=expect+(X0(f+1))*(f-0.5);
end;
expect=expect/(1-X0(11));         
delay=expect*dl+dl;        

power=0;
power1=0;
power2=0;
power3=0;
power4=0;
power5=0;
power6=0;

txp=0.059/1000;
rxp=0.052/1000;


power6=0;
P6=0;
%for act=1:active
    for t=1:active %T
        for ii=0:N-1-1
            % i nodes wake up before t, but have nothing to send
            a=nchoosek(N-1,ii)*((t-1)/T)^ii*pp0^ii;
            % N-1-i nodes wake up from t, have pkts to send =
            for j=1:N-1-ii
                % j nodes wake up at t
                b=nchoosek(N-1-ii,j)*(1/T)^j;
                for k=1:j
                    % k nodes in j nodes have pkt to send
                    c=nchoosek(j,k)*pp0^(j-k)*(1-pp0)^k;
                    % N-i-j nodes wake up from t+1
                    d=((T-t)/T)^(N-1-ii-j);
                    e=pp0^(N*0);
                    power6=power6+a*b*c*d*e*(rxp*t+(ADV+(ADV+ACK)/2)*rxp);
                    P6=P6+a*b*c*d*e;
                end;
            end;
        end;    
    end;
%end;
power6=power6/P6;

    
power1=(1-pp0)*ps*(DATA*txp+((T/2)/(ADV+ACK))*(ADV*txp+ACK*rxp));
power2=(1-pp0)*ps*((ADV+ACK)/2*rxp+ADV*rxp+ACK*txp+DATA*rxp);

power3=(1-pp0)*pf*(T/(ADV+ACK))*(ADV*txp+ACK*rxp);
power4=(1-pp0)*pf*(ADV+(ADV+ACK)/2)*rxp;

%power5=(1-2*(1-pp0)*p-Pnb)*(ADV+(ADV+ACK)/2)*rxp;
%power6=Pnb*active*rxp;
power5=(1-2*(1-pp0)*p)*power6;

%power=power1+power2+power3+power4+power5+power6;
power=power1+power2+power3+power4+power5;

PDR
ps
pf
pp0
delay
power
