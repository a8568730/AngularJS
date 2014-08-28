#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
//#include <direct.h>
//#include "type_define.h"
//#include "variable.h"
#include "variable.c"
#include "feature.h"
//#include "tcos.h"
#include "tcos.c"

char IN_WAV_DIR[256];
char OUT_C12_DIR[256];
char OUT_EC_DIR[256];
float C0;
//#define   IN_WAV_DIR    "/home1/e0mars5/santu/Feature/MAT2000/TR/Waveform/Clean/"
//#define   OUT_C12_DIR   "/home1/e0mars5/santu/Feature/MAT2000/TR/Clean/MFCC_C12"
//#define   OUT_EC_DIR    "/home1/e0mars5/santu/Feature/MAT2000/TR/Clean/MFCC_ENG"

/***********************************
*        make directory
***********************************/
   int MakeDirectory(char *filename)
   {
      int len, i;
      char pathfilename[512],newDir[512],currentPath[512],*ptr;
      char buffer[512];

      len=0; ptr=pathfilename;
      while (filename[len]!='\0')
      {
         if (filename[len]=='/' || filename[len]=='\\')
         {
            //filename[len]='\\'; //run in window os, add #include <direct.h>
            *ptr=0;
            //  _mkdir(pathfilename);
            getcwd(currentPath,512);
            for(i=0; i<len; i++)
               newDir[i] = filename[i];
            newDir[i] = '\0';
            if(chdir(newDir)!=0)
            {
               chdir(currentPath);
               if(strlen(pathfilename)>0)
               {
                  sprintf(buffer,"mkdir %s",pathfilename);
                  system(buffer);
                  // or do //mkdir(pathfilename);
               }
            }
            chdir(currentPath);
         }
         *ptr++=filename[len++];
      }
      return(1);
   }
int filesize(FILE *stream)
{
   long curpos, length;

   curpos = ftell(stream);
   fseek(stream, 0L, SEEK_END);
   length = ftell(stream);
   fseek(stream, curpos, SEEK_SET);

   return (int)length;
}
void CEPLIFTER(int Frames)
{
	int i,j;
	float tmpf;
	float a,Lby2;    
	    
	    a = PI/22;
   Lby2 = 22/2.0;
   for(i=0;i<Frames;i++)
   {
      	for(j=0;j<CEP_DIM;++j)
      	{
      		//printf("t=%d j=%d %f ",i,j,gb_cep_data[i][j]);
      		tmpf=1.0 + Lby2*sin((j+1) * a);
        gb_cep_data[i][j]*=tmpf;
        //printf("%f\n",gb_cep_data[i][j]);
        //getchar();
      } 
    }
}
float Mel(int k)
{
	float	res;
	
	res = (float)1.0E7/(float)(SAMPPERIOD*FFT_ORDER*700.0);
	return ((float)(log(1+(k-1)*res)*1127.0));
}

void SetFilterBanks(double w[])
{
	int	b,Banks,n,curnB;
	float scale,melfreq;
	float centerF[FFT_ORDER_2];
	scale = Mel(FFT_ORDER_2+1);
	Banks = FILTERNO+1;
	for (b=0; b < Banks; b++)
	{
		centerF[b] = ((float)(b+1)/(float)Banks)*scale;
	}
	
	curnB = 0;
	for (n=0; n < FFT_ORDER_2; n++)
	{
		melfreq = Mel(n+1);
		gb_bank[n] = curnB;
		if (melfreq > centerF[curnB])
		{
			curnB++;
			gb_bank[n] = curnB;
		}
		if (curnB > 0)
		{
			w[n] = (centerF[curnB]-melfreq)/(centerF[curnB]-centerF[curnB-1]);
		}
		else
		{
			w[n] = (centerF[0]-melfreq)/centerF[0];
		}
	}
	return;
}

void PreEmphasis(float s[])
{
  int i;

  for (i=FRAME_SIZE-1;i>0;i--)
  {
     s[i] -= (s[i-1]*(float)ALPHA);
  }
  s[0] *= ((float)1.0-(float)ALPHA);
  return;
}

void GetHammingWindow()
{
     int    n;
     double temp;
     temp = (2.0 * PI) / (FRAME_SIZE - 1);
     for (n=0; n<FRAME_SIZE; n++)
     {
         HW[n] = (float)(0.54 - 0.46*cos((double)n*temp));
         
     }
}

void Haming_window(float s[])
{
  int i;
  float temp;
  for (i=0;i < FRAME_SIZE;i++)
  {
      temp = (float)(s[i]*HW[i]);
	  s[i] = temp ;
  }
  return;
}

void fft(int inv,int n,COMPLEX *z_data)
{
   int	m,
			le,
			ip,
			i,
			l,
			nm1,
			j,
			le1,
			nv2,
			k;
   COMPLEX u,ti,w;
   double	tmpx,tmpy;
   int fft_order;
   fft_order=FFT_ORDER;

   m=0;
   while(fft_order>1){
	   fft_order>>=1;
	   m++;
   }
	
   nv2=(int)((double)n/2.0);
   nm1=n-1;
   j=1;
   for(i=1;i<=nm1;++i)
   {
      if (i<j)
      {
		 tmpx=z_data[j].x;
         tmpy=z_data[j].y;
         z_data[j].x=z_data[i].x;
         z_data[j].y=z_data[i].y;
		 z_data[i].x=tmpx;
         z_data[i].y=tmpy;
      }
      k=nv2;
      while (k<j)
      {
         j=j-k;
         k=(int)((double)k/2.0);
      }
      j=j+k;
   }
   for(l=1;l<=m;++l)
   {
      le=(int)pow(2.0,(double)l);
      le1=(int)((double)le/2.0);
      u.x=1.0;
      u.y=0.0;
      w.x= cos(PI/(double)le1);
      w.y= sin(PI/(double)le1)*(-inv);
      for(j=1;j<=le1;++j)
      {
         i=j;
         while (i<=n)
         {
			 ip=i+le1;
			 ti.x=z_data[ip].x*u.x-z_data[ip].y*u.y;
			 ti.y=z_data[ip].x*u.y+z_data[ip].y*u.x;
			 z_data[ip].x=z_data[i].x-ti.x;
			 z_data[ip].y=z_data[i].y-ti.y;
			 z_data[i].x=z_data[i].x+ti.x;
			 z_data[i].y=z_data[i].y+ti.y;
			 i=i+le;
         }
		 tmpx=u.x;
		 u.x=tmpx*w.x-u.y*w.y;
		 u.y=tmpx*w.y+u.y*w.x;
      }
   }
   if (inv== -1)
   {
      for (i=1;i<=n;++i)
      {
			z_data[i].x=z_data[i].x/(double)n;
			z_data[i].y=z_data[i].y/(double)n;
      }
   }
   return;
}
void SubDC(float s[])
{
	double sum,y,mean;
	int i;
	sum=0;
	for(i=0;i<FRAME_SIZE;i++)
	{
		sum+=s[i];
	}
	mean=sum/(float)FRAME_SIZE;

	for(i=0;i<FRAME_SIZE;i++)
	{
		//printf("%f\n",s[i]);
		y=(double)s[i]-mean;
		s[i]=(float)y;
				//printf("%f\n",s[i]);
//getchar();
	}
}
int MelBandMagnitude(short *sample)
{
   int	k,i,j,t,b,wait,errorCode;
   float	abs_z,
			pb,
			pb1;

	double power_z;

	double sum;

	float s[FRAME_SIZE]; 
    COMPLEX z[FFT_ORDER+1];
	int totalFrames;
	int frameSize,frameRate;

	errorCode=1;
    frameRate=FRAME_SHIFT;
    frameSize=FRAME_SIZE;
	//gb_totalFrames = (sampleNumber-(FRAME_SIZE-FRAME_SHIFT))/FRAME_SHIFT;
    totalFrames=gb_totalFrames;
	wait=-1;

   for (t=0; t<totalFrames; t++)
   {
	  sum = 0.0;
      for (j=0; j<frameSize; j++) 
		{
         s[j] = (float)sample[t*frameRate+j];
         sum += s[j]*s[j];
      }

	  if(sum == 0)
	  {
		  if(wait == -1)
		  {
			  wait=t;
		  }
	  }
	  else
	  {
		  gb_eng[t] = (float)10.0*(float)log10(sum/(float)frameSize);
		  if(wait != -1)
		  {
			  for(j=wait;j < t;j++)
			  {
				  gb_eng[j]=gb_eng[t];
			  }
			  wait=-1;
		  }
	  }

      SubDC(s);
	  PreEmphasis(s);
      Haming_window(s);

      for (i=0;i <FFT_ORDER;i++)
      {
         z[i+1].x = 0.0;
         z[i+1].y = 0.0;
      }
      for (i=0;i < frameSize;i++)
         z[i+1].x = s[i];

      fft(1,FFT_ORDER,z);

      for (b=1; b <= FILTERNO; b++)
	      gb_band_eng[t][b] = (float)0.0;
		
	  
      for (k=2; k <= FFT_ORDER_2; k++)
      {
		 power_z = z[k].x*z[k].x+z[k].y*z[k].y;
         abs_z = (float)sqrt(power_z);
         b = gb_bank[k-1];
         pb = (float)(gb_fft_wt[k-1]*abs_z);
		 pb1 = (float)(gb_fft_wt[k-1]*power_z);
         if (b > 0)
		 {
            
			gb_band_eng[t][b]+=pb;

		 }
         if (b < FILTERNO)
		 {
            gb_band_eng[t][b+1] += abs_z-pb;

		 }
      }
	  
      
   }//for (t=0; t<totalFrames; t++)


   return(errorCode);
}


void DCT(CEP_DATA cep[])
{
	int i,j,t;
	double pp;
	double temp;
	int totalFrames;
    float power[FILTERNO+1];
	float mfnorm;
	totalFrames=gb_totalFrames;
	mfnorm = (float)sqrt(2.0/((float)FILTERNO));
	for(t=0;t<totalFrames;t++)
    {
		temp=0;
		for(i=0;i<=FILTERNO;i++)
			power[i]=(float)gb_band_eng[t][i];
		
		if(EC){
			EnergyContour(&gb_eng[t],gb_band_eng[t]);
		}
		for(i=1;i<=FILTERNO;i++)
        {
			if(gb_band_eng[t][i]<1.0) gb_band_eng[t][i]=1.0;
			gb_band_eng[t][i] = log(gb_band_eng[t][i]);
		} 
		for (i=0; i < CEP_DIM; i++)
		{
			pp = (float)0.0;
			for (j=1; j <= FILTERNO; j++)
			{
				pp += gb_band_eng[t][j]*tcos[i+1][j];
				//tcos[][] are pre-computed values of cos(PI*(i+1)/24*(j-0.5))
			}
			pp *= mfnorm;
			cep[t][i]=(float)pp;
		}
		C0 = 0.0;
		for (j=1; j <= FILTERNO; j++)
		{
			C0 += gb_band_eng[t][j];
			//tcos[][] are pre-computed values of cos(PI*(i+1)/24*(j-0.5))
		}
		C0*=mfnorm;
		cep[t][CEP_DIM]=C0;
		//printf("%f %f\n",C0,cep[t][CEP_DIM]);
		
		//fpmc
		/*
		C0[t]=0;
		for(j=1;j<=FILTERNO;j++) C0[t]+=DCT[0][j-1]*gb_band_eng[t][j];
		*/
		
	} // loop t
}

void EnergyContour(float* eng,double *pwr)
{
   int b;
   double temp;
   temp=0;
   for(b=6;b<=FILTERNO;b++){
	   temp+=pwr[b];
   }
   *eng=(float)10.0*(float)log10(temp);
   
}

//Romove long term average of power spectrum
void LTA222()
{
  int i,j,k,m,dis_no;
  double mf[FILTERNO+1],tf,thld[FILTERNO+1];
  int frm_no;
  frm_no=gb_totalFrames;
  
  for(i=0;i<=FILTERNO;i++) mf[i]=0.0;
  
  m=0;
  if(CUTPURENOISE) dis_no=NOISE_FRM;
  else dis_no=0;
  
  for(i=dis_no;i<frm_no-dis_no;i++)
  {
      m++;
      for(j=1;j<=FILTERNO;j++) mf[j]+=gb_band_eng[i][j]*gb_band_eng[i][j];
  }
  for(i=1;i<=FILTERNO;i++)
  {
      mf[i]/=(double) m;
      thld[i]=ALPHA_LTA/(1.0-BETA)*mf[i];
  } 
  k=m=0;   
  for(i=0;i<frm_no;i++)
  {
	  
	  for(j=1;j<=FILTERNO;j++) 
	  {
		  tf=gb_band_eng[i][j]*gb_band_eng[i][j];
		  if(tf>thld[j])
		  { 
			  tf=tf-ALPHA_LTA*mf[j]; 
			  if(i>=dis_no&&i<frm_no-dis_no) 
				  k++; 
			  else 
				  m++; 
		  }
		  else 
			  tf*=BETA;
		  
		  gb_band_eng[i][j]=sqrt(tf);
	  }
  }  
} // end of LTA()

int getTotalFrame(int sampleNumber)
{
	return((sampleNumber-(FRAME_SIZE-FRAME_SHIFT))/FRAME_SHIFT);
}

void ini_fea_para()
{
  SetFilterBanks(gb_fft_wt);
  GetHammingWindow();
}

void getDeltaCep(int totalFrames,CEP_DATA *cep,CEP_DATA *deltaCep)
{
	int	k,i,t,lastT;
   float sigmaT2,sum;

	//Compute delta cepstrum
   sigmaT2 = (float)0.0;
   for (t=1; t<=DELTA; t++)
	  sigmaT2 += t*t;
	
   sigmaT2 *= (float)2.0;

   //frome frame 0 to frame DELTA
   for (t=0; t < DELTA; t++) 
	{
      for (i=0; i < CEP_DIM; i++)
		{
         deltaCep[t][i]=cep[t+1][i] - cep[t][i];
		}
   }

   //between DELTA to Tot - DELTA frames
   lastT = totalFrames - DELTA;
   for (t=DELTA; t < lastT; t++)
   {
      for (i=0; i < CEP_DIM; i++)
      {
         sum = (float)0.0;
         for (k=1; k <= DELTA; k++)
		 {
            sum += k*(cep[t+k][i]-cep[t-k][i]);
		 }
         deltaCep[t][i]=sum/sigmaT2;
      }
   }

   //from frame Tot - DELTA to frame Tot
   for (t=lastT; t < totalFrames; t++) 
   {
      for (i=0; i < CEP_DIM; i++)
	  {
         deltaCep[t][i]=cep[t][i]-cep[t-1][i];
	  }
   }
	for (t=0; t < totalFrames; t++) 
   {
      for (i=0; i < CEP_DIM; i++)
	  {
        printf("t=%d D=%d f=%f\n",t,i, deltaCep[t][i]);
        getchar();
	  }
   }
	return;
}
void getDeltaCep_New(int totalFrames,CEP_DATA *cep,CEP_DATA *deltaCep)
{
	int	k,i,t,lastT;
   float sigmaT2,sum;
   int index;

	//Compute delta cepstrum
   sigmaT2 = (float)0.0;
   for (t=1; t<=DELTA; t++)
	  sigmaT2 += t*t;
	
   sigmaT2 *= (float)2.0;

   for (t=0; t < totalFrames; t++)
   {
	   for (i=0; i < CEP_DIM+1; i++)
	   {
		   sum = (float)0.0;
		   for (k=1; k <= DELTA; k++)
		   {
			   if(t-k <0)
			   {
				   sum += k*(cep[t+k][i]-cep[0][i]);
			   }
			   else if(t+k >=totalFrames)
			   {
				   sum += k*(cep[totalFrames-1][i]-cep[t-k][i]);
			   }
			   else
			   {
				   sum += k*(cep[t+k][i]-cep[t-k][i]);
			   }
		   }
		   deltaCep[t][i]=sum/sigmaT2;
	   }
   }
   /*
for (t=0; t < totalFrames; t++) 
   {
      for (i=0; i < CEP_DIM+1; i++)
	  {
        printf("t=%d D=%d f=%f %f\n",t,i, cep[t][i],deltaCep[t][i]);
        getchar();
	  }
   }*/
	return;
}
void getDDEng(int totalFrames,float *eng,float *dEng,float *ddEng)
{
   int	k,t,lastT;
   float sigmaT2,sum;

	//Compute delta cepstrum
   sigmaT2 = (float)0.0;
   for (t=1; t<=DELTA; t++)
	   sigmaT2 += t*t;
	sigmaT2 *= (float)2.0;

   //frome frame 0 to frame DELTA
   for (t=0; t < DELTA; t++) 
     dEng[t]=eng[t+1] - eng[t];
  

   //between DELTA to Tot - DELTA frames
   lastT = totalFrames - DELTA;
   for (t=DELTA; t < lastT; t++)
   {
      sum = (float)0.0;
      for (k=1; k <= DELTA; k++)
	  {
         sum += k*(eng[t+k]-eng[t-k]);
	  }
      dEng[t]=sum/sigmaT2;
   }

   //from frame Tot - DELTA to frame Tot
   for (t=lastT; t < totalFrames; t++) 
   {
      dEng[t]=eng[t]-eng[t-1];
   }
	
	//delta delta energy
   for (t=0; t < DDELTA; t++)
   {
      ddEng[t]=dEng[t+1] - dEng[t];
   }

   lastT = totalFrames - DDELTA;
   for (t=DDELTA; t < lastT; t++)
   {
      ddEng[t]=dEng[t+1]-dEng[t-1];
   }

   for (t=lastT; t < totalFrames; t++)
   {
      ddEng[t]=dEng[t]-dEng[t-1];
   }

	return;
}


//int MFCC_shell(char *fname,FEA_DATA fea[])
int MFCC_shell(char *fname)
{ 
   FILE *fp;
   short *wave, *tmpWave;
   int samples_no;
   int t,d;
   int begint,endt;
   int frm_no;

   double  sample_mean;
   short   headerLength;

   int     max_samples_no, max_gb_totalFrames, K_pt, shiftN;
   int     *tmpFrame;
   float   *tmpEng;

   CEP_DATA /* *cep_data, */ *dcep_data, *tmpCep_data;
 
   fp=fopen(fname,"rb");
   if(!fp) {
	   printf("can't open file %s\n",fname);
	   exit(1);
   }

   //samples_no=filesize(fp);
   wave=calloc(sizeof(short),MAX_SAMPLE_NO);

   if(WAV_VAT==1)
   {//VAT file header
      fread(&headerLength,sizeof(short),1,fp);
      if (headerLength==1)	
      {
         fseek(fp,256,SEEK_SET);
      }
      else if (headerLength==2) 
      {
         fseek(fp,512,SEEK_SET); 
      }
   }
   else
   {//WAV file
   	fseek(fp, FILE_HEADER_LEN, SEEK_SET);
   }

   samples_no=fread(wave,sizeof(short),MAX_SAMPLE_NO,fp);
   gb_totalFrames=getTotalFrame(samples_no);
   //cep_data=calloc(sizeof(CEP_DATA),gb_totalFrames);
   gb_cep_data=calloc(sizeof(CEP_DATA),gb_totalFrames);
   //dcep_data=calloc(sizeof(CEP_DATA),gb_totalFrames);
   
/*
   //DC substration
   sample_mean = 0;
   for(t=0; t<samples_no; t++)
   {
      sample_mean += (double)wave[t];
   }
   sample_mean /= (double)samples_no;
   for(t=0; t<samples_no; t++)
   {
      wave[t] -= (short)sample_mean;
   }
*/

   //initialize
	max_samples_no = samples_no;
	max_gb_totalFrames = gb_totalFrames;

	tmpCep_data=calloc(sizeof(CEP_DATA),max_gb_totalFrames);
	if(tmpCep_data==NULL)
	{
		printf("memory allocate error\n");		
	}
	tmpFrame = malloc(sizeof(int)*K_POINT);
	if(tmpFrame==NULL) 
	{
		printf("memory allocate error\n");		
	}
	tmpEng   = malloc(sizeof(float)*max_gb_totalFrames);
	if(tmpEng==NULL) 
	{
		printf("memory allocate error\n");
	}

	for (t=0; t<max_gb_totalFrames; t++)
	{
		tmpEng[t] = 0.0f;
		for (d=0; d<CEP_DIM; d++)
			tmpCep_data[t][d] = 0.0f;
	}
	
	for(K_pt=K_POINT-1; K_pt>=0; K_pt--)
	{
		shiftN = (int)K_pt*((int)FRAME_SHIFT/(float)K_POINT);
		
		tmpWave = wave+shiftN;
		samples_no = max_samples_no - shiftN;
		gb_totalFrames=getTotalFrame(samples_no);
		tmpFrame[K_pt] = gb_totalFrames;
		
		//neet gb_totalFrames, FRAME_SHIFT, FRAME_SIZE,  get gb_band_eng[t][b+1]
		MelBandMagnitude(tmpWave);
		if(LTA)
		{//need gb_band_eng[t][b+1], gb_totalFrames, modify gb_band_eng[t][b+1]
			LTA222(); 
		}	   
		DCT(gb_cep_data);
		
		
		//sum gb_cep_data
		for (t=0; t<gb_totalFrames; t++)
		{
			tmpEng[t] += gb_eng[t];
			for (d=0; d<CEP_DIM; d++)
				tmpCep_data[t][d] += gb_cep_data[t][d];
		}
	}

   //avg. tmpCep_data[t][d], tmpEng[t] -> gb_cep_data[t][d], gb_eng[t]
	samples_no = max_samples_no;
	gb_totalFrames = max_gb_totalFrames;
	for (t=0; t<gb_totalFrames; t++)
	{
		if(t != gb_totalFrames-1 || tmpFrame[K_POINT-1] == gb_totalFrames)
		{//do for last frame
			gb_eng[t] = tmpEng[t]/(float)K_POINT;
			for (d=0; d<CEP_DIM; d++)
				gb_cep_data[t][d] = tmpCep_data[t][d]/(float)K_POINT;
		}
		else //use the last time result: gb_eng[t], gb_cep_data[t][d]
		{ 
			//printf(" frame have less\n");
		}
	}

   
   if(CUTPURENOISE)
   {
	   begint=NOISE_FRM;
	   endt=gb_totalFrames-NOISE_FRM;
	   
   }
   else
   {
   	   begint=0;
	   endt=gb_totalFrames;
   }
   frm_no=endt-begint;


/*
   CMN(cep_data,begint,endt,frm_no);
   getDeltaCep(gb_totalFrames,cep_data,dcep_data);
   getDDEng(gb_totalFrames,gb_eng,gb_deng,gb_ddeng);
   for(t=0;t<gb_totalFrames;t++){
	  for(d=0;d<CEP_DIM;d++)
        fea[t][d]=cep_data[t][d];
	  
	  for(d=0;d<CEP_DIM;d++)
        fea[t][d+CEP_DIM]=dcep_data[t][d];
      
	  fea[t][DIM-2]=gb_deng[t];
	  fea[t][DIM-1]=gb_ddeng[t];

   }
*/
   //free(cep_data);
   //if(dcep_data)
   	//free(dcep_data);
   
   if(wave)
	   	free(wave);
   if(tmpCep_data)
		free(tmpCep_data);
   if(tmpEng)
	   free(tmpEng);
   if(tmpFrame)
	   free(tmpFrame);
   if(fp)
   		fclose(fp);

   return gb_totalFrames;
}

void CMN(CEP_DATA *Cep_Data,int begin,int end,int frameNumber)
{
	int	i,t;
	double cmean[CEP_DIM];
    
	//cepstrum mean normalize
	printf("%d %d %d\n",begin,end,frameNumber);
	for (i=0; i<CEP_DIM ;i++)
	{
		cmean[i] = 0.0f;
		for (t=begin; t<end; t++)
		{
			cmean[i] += Cep_Data[t][i];
		}
		cmean[i] /= (float)frameNumber;
	}
   
   for (t=0; t<frameNumber+2*NOISE_FRM; t++)
	{
		for (i=0; i<CEP_DIM; i++)
		{
			printf("%f %f\n",Cep_Data[t][i],cmean[i]);
			Cep_Data[t][i] -= cmean[i];    
			printf("%f %f\n",Cep_Data[t][i],cmean[i]);
			getchar();
		}
	}

	return;
}
void MVN(OUT_DATA *Cep_Data,int frameNumber)
{
   int   t, d;
   double mean[OUT_DIM], sd[OUT_DIM];

   int   i;
   double *fea;

   for (d=0; d<OUT_DIM; ++d)
   {
      mean[d] = 0.0f;
      sd[d] = 0.0f;
   }

   for (t=0; t<frameNumber; ++t)
      for (d=0; d<OUT_DIM; ++d)
      {
         mean[d] += Cep_Data[t][d];
         sd[d]  += Cep_Data[t][d]*Cep_Data[t][d];
      }

   for (d=0; d<OUT_DIM; ++d)
   {
      mean[d] /= (float)frameNumber;
      sd[d] = (float)sqrt(sd[d]/(float)frameNumber - mean[d]*mean[d]);
   }

   for (t=0; t<frameNumber; ++t)
      for (d=0; d<OUT_DIM; ++d)
         Cep_Data[t][d] = (Cep_Data[t][d] - mean[d])/sd[d];
   
   if(DO_AVG==1)
   {
      fea = (double *) malloc(sizeof(double)*frameNumber);
      for(d=0; d<OUT_DIM; d++)
      {
         for (t=ORDER_M; t<(frameNumber-ORDER_M); ++t)
         {
           fea[t] = 0.0f;
           for(i=(-ORDER_M); i<=ORDER_M; i++)
              fea[t] += Cep_Data[t+i][d];

      	    fea[t] /= (double)(2.0f*ORDER_M+1.0f);
         }

         //assign Cep_Data[t][d]
         for (t=ORDER_M+1; t<(frameNumber-ORDER_M); ++t)
            Cep_Data[t][d] = (float)fea[t];
      }
      if(fea)
      {
         free(fea);
         fea = NULL;
      }
   }
   else if(DO_AVG==2)
   {
      //do ARMA
      fea = (double *) malloc(sizeof(double)*1);
      for(d=0; d<OUT_DIM; d++)
      {
         for (t=ORDER_M; t<(frameNumber-ORDER_M); ++t)
         {
            *fea = 0.0f;
	    for(i=1; i<=ORDER_M; i++)
	    {
               (*fea) += Cep_Data[t+i][d];
               (*fea) += Cep_Data[t-i][d];
            }
            Cep_Data[t][d] += (float)(*fea);
      	    Cep_Data[t][d] /= (float)(2.0f*ORDER_M+1.0f);
         }
      }
      if(fea)
      {
         free(fea);
         fea = NULL;
      }

   }
}

void MVN2(OUT_DATA *Cep_Data,int frameNumber, int window)
{
   int   t, d;
   double mean[OUT_DIM], sd[OUT_DIM];
   double mean2[frameNumber][OUT_DIM], sd2[frameNumber][OUT_DIM];

   int   i;
   double *fea;
   int tt,t_index;
   
   printf("qtk total frame:%d\n",frameNumber);getchar();

   for (d=0; d<OUT_DIM; ++d)
   {
      mean[d] = 0.0f;
      sd[d] = 0.0f;
   }

   for (t=0; t<frameNumber; ++t)
      for (d=0; d<OUT_DIM; ++d)
      {
         mean[d] += Cep_Data[t][d];
         sd[d]  += Cep_Data[t][d]*Cep_Data[t][d];
      }

   for (d=0; d<OUT_DIM; ++d)
   {
      mean[d] /= (float)frameNumber;
      sd[d] = (float)sqrt(sd[d]/(float)frameNumber - mean[d]*mean[d]);
   }

   //dynamic mean & std calculation
   for (t=0; t<frameNumber; ++t)
   {
   
      for (d=0; d<OUT_DIM; ++d)
          mean2[t][d] = sd2[t][d] = 0.0f;
          
      for (tt=t-window;tt<=t+window;tt++)
      {
           if(tt < 0)
           {
              //t_index=0;
              if(frameNumber+tt<0)
                t_index =0;
              else              
                t_index = frameNumber+tt;
           }
           else if(tt > frameNumber-1)
           {
              //t_index=frameNumber-1;
              if(tt-frameNumber > frameNumber-1)
                 t_index = frameNumber-1;
              else
                 t_index = tt-frameNumber;
           }
           else
              t_index=tt;
              
              
           for (d=0; d<OUT_DIM; ++d)
           {
              mean2[t][d] += Cep_Data[t_index][d];
              sd2[t][d] += Cep_Data[t_index][d]*Cep_Data[t_index][d];
           }
      }
      
      for (d=0; d<OUT_DIM; ++d)
      {
          mean2[t][d] /= (float)(2.0f*window+1.0f);
          sd2[t][d] = (float)sqrt(sd2[t][d]/(float)(2.0f*window+1.0f) - mean2[t][d]*mean2[t][d]);
      }
   
      //printf("ok frame:%d mean2 sd2\n",t);
          
   }
   //printf("qtk! ok mean2 sd2!\n");
   
   //using dynamic mean & std
   for (t=0; t<frameNumber; ++t)
     for (d=0; d<OUT_DIM; ++d)
        Cep_Data[t][d] = (Cep_Data[t][d] - mean2[t][d])/sd2[t][d];   

//using whole sentence
/*
   for (t=0; t<frameNumber; ++t)
      for (d=0; d<OUT_DIM; ++d)
         Cep_Data[t][d] = (Cep_Data[t][d] - mean[d])/sd[d];
*/ 
        
   
   if(DO_AVG==1)
   {
      fea = (double *) malloc(sizeof(double)*frameNumber);
      for(d=0; d<OUT_DIM; d++)
      {
         for (t=ORDER_M; t<(frameNumber-ORDER_M); ++t)
         {
           fea[t] = 0.0f;
           for(i=(-ORDER_M); i<=ORDER_M; i++)
              fea[t] += Cep_Data[t+i][d];

      	    fea[t] /= (double)(2.0f*ORDER_M+1.0f);
         }

         //assign Cep_Data[t][d]
         for (t=ORDER_M+1; t<(frameNumber-ORDER_M); ++t)
            Cep_Data[t][d] = (float)fea[t];
      }
      if(fea)
      {
         free(fea);
         fea = NULL;
      }
   }
   else if(DO_AVG==2)
   {
      //do ARMA
      fea = (double *) malloc(sizeof(double)*1);
      for(d=0; d<OUT_DIM; d++)
      {
         for (t=ORDER_M; t<(frameNumber-ORDER_M); ++t)
         {
            *fea = 0.0f;
	    for(i=1; i<=ORDER_M; i++)
	    {
               (*fea) += Cep_Data[t+i][d];
               (*fea) += Cep_Data[t-i][d];
            }
            Cep_Data[t][d] += (float)(*fea);
      	    Cep_Data[t][d] /= (float)(2.0f*ORDER_M+1.0f);
         }
      }
      if(fea)
      {
         free(fea);
         fea = NULL;
      }

   }
}

//2013/12/3, padding tail frame: fixed by buff for tail(buff=MV_WINDOW*2+1)
void MVN2_T1(OUT_DATA *Cep_Data,int frameNumber, int window)
{
   int   t, d;
   double mean[OUT_DIM], sd[OUT_DIM];
   double mean2[frameNumber][OUT_DIM], sd2[frameNumber][OUT_DIM];

   int   i;
   double *fea;
   int tt,t_index,f_flag;
   
   //printf("qtk total frame:%d\n",frameNumber);getchar();

   //dynamic mean & std calculation
   f_flag=0;
   for (t=0; t<frameNumber; ++t)
   {
      //track last buff at tail
	  if (t==frameNumber-window)
		f_flag=1;
		
	  for (d=0; d<OUT_DIM; ++d)
          mean2[t][d] = sd2[t][d] = 0.0f;
          
      if (f_flag)
	  {
		  for (tt=frameNumber-window*2;tt<frameNumber;tt++)
		  {
			   if(tt < 0)
			   {
				  //t_index=0;
				  if(frameNumber+tt<0)
					t_index =0;
				  else              
					t_index = frameNumber+tt;
			   }
			   else if(tt > frameNumber-1)
			   {
				  //t_index=frameNumber-1;
				  if(tt-frameNumber > frameNumber-1)
					 t_index = frameNumber-1;
				  else
					 t_index = tt-frameNumber;
			   }
			   else
				  t_index=tt;
				  
				  
			   for (d=0; d<OUT_DIM; ++d)
			   {
				  mean2[t][d] += Cep_Data[t_index][d];
				  sd2[t][d] += Cep_Data[t_index][d]*Cep_Data[t_index][d];
			   }
		  }
	  }
	  else
	  {
		  for (tt=t-window;tt<=t+window;tt++)
		  {
			   if(tt < 0)
			   {
				  //t_index=0;
				  if(frameNumber+tt<0)
					t_index =0;
				  else              
					t_index = frameNumber+tt;
			   }
			   else if(tt > frameNumber-1)
			   {
				  //t_index=frameNumber-1;
				  if(tt-frameNumber > frameNumber-1)
					 t_index = frameNumber-1;
				  else
					 t_index = tt-frameNumber;
			   }
			   else
				  t_index=tt;
				  
				  
			   for (d=0; d<OUT_DIM; ++d)
			   {
				  mean2[t][d] += Cep_Data[t_index][d];
				  sd2[t][d] += Cep_Data[t_index][d]*Cep_Data[t_index][d];
			   }
		  }
	  }

      
      for (d=0; d<OUT_DIM; ++d)
      {
          mean2[t][d] /= (float)(2.0f*window+1.0f);
          sd2[t][d] = (float)sqrt(sd2[t][d]/(float)(2.0f*window+1.0f) - mean2[t][d]*mean2[t][d]);
      }
   
      //printf("ok frame:%d mean2 sd2\n",t);
          
   }
   //printf("qtk! ok mean2 sd2!\n");
   
   //using dynamic mean & std
   for (t=0; t<frameNumber; ++t)
     for (d=0; d<OUT_DIM; ++d)
        Cep_Data[t][d] = (Cep_Data[t][d] - mean2[t][d])/sd2[t][d];   

//using whole sentence
/*
   for (d=0; d<OUT_DIM; ++d)
   {
      mean[d] = 0.0f;
      sd[d] = 0.0f;
   }

   for (t=0; t<frameNumber; ++t)
      for (d=0; d<OUT_DIM; ++d)
      {
         mean[d] += Cep_Data[t][d];
         sd[d]  += Cep_Data[t][d]*Cep_Data[t][d];
      }

   for (d=0; d<OUT_DIM; ++d)
   {
      mean[d] /= (float)frameNumber;
      sd[d] = (float)sqrt(sd[d]/(float)frameNumber - mean[d]*mean[d]);
   }


   for (t=0; t<frameNumber; ++t)
      for (d=0; d<OUT_DIM; ++d)
         Cep_Data[t][d] = (Cep_Data[t][d] - mean[d])/sd[d];
*/ 
        
   
   if(DO_AVG==1)
   {
      fea = (double *) malloc(sizeof(double)*frameNumber);
      for(d=0; d<OUT_DIM; d++)
      {
         for (t=ORDER_M; t<(frameNumber-ORDER_M); ++t)
         {
           fea[t] = 0.0f;
           for(i=(-ORDER_M); i<=ORDER_M; i++)
              fea[t] += Cep_Data[t+i][d];

      	    fea[t] /= (double)(2.0f*ORDER_M+1.0f);
         }

         //assign Cep_Data[t][d]
         for (t=ORDER_M+1; t<(frameNumber-ORDER_M); ++t)
            Cep_Data[t][d] = (float)fea[t];
      }
      if(fea)
      {
         free(fea);
         fea = NULL;
      }
   }
   else if(DO_AVG==2)
   {
      //do ARMA
      fea = (double *) malloc(sizeof(double)*1);
      for(d=0; d<OUT_DIM; d++)
      {
         for (t=ORDER_M; t<(frameNumber-ORDER_M); ++t)
         {
            *fea = 0.0f;
	    for(i=1; i<=ORDER_M; i++)
	    {
               (*fea) += Cep_Data[t+i][d];
               (*fea) += Cep_Data[t-i][d];
            }
            Cep_Data[t][d] += (float)(*fea);
      	    Cep_Data[t][d] /= (float)(2.0f*ORDER_M+1.0f);
         }
      }
      if(fea)
      {
         free(fea);
         fea = NULL;
      }

   }
}


//2013/12/3, padding tail frame: fixed by whole sentence for tail(buff=MV_WINDOW*2+1)
void MVN2_T2(OUT_DATA *Cep_Data,int frameNumber, int window)
{
   int   t, d;
   double mean[OUT_DIM], sd[OUT_DIM];
   double mean2[frameNumber][OUT_DIM], sd2[frameNumber][OUT_DIM];

   int   i;
   double *fea;
   int tt,t_index,f_flag;
   
   //printf("qtk total frame:%d\n",frameNumber);getchar();
   for (d=0; d<OUT_DIM; ++d)
   {
      mean[d] = 0.0f;
      sd[d] = 0.0f;
   }

   for (t=0; t<frameNumber; ++t)
      for (d=0; d<OUT_DIM; ++d)
      {
         mean[d] += Cep_Data[t][d];
         sd[d]  += Cep_Data[t][d]*Cep_Data[t][d];
      }


   //dynamic mean & std calculation
   f_flag=0;
   for (t=0; t<frameNumber; ++t)
   {
      //track last buff at tail
	  if (t==frameNumber-window)
		f_flag=1;
		
	  for (d=0; d<OUT_DIM; ++d)
          mean2[t][d] = sd2[t][d] = 0.0f;
          
      if (f_flag)
	  {
		//When trace index (t) falls in intervals of windows w.r.t end frame, 
		//directly take mean & var of the total frames rather than of frames between windows
		  for (d=0; d<OUT_DIM; ++d)
		  {
			mean2[t][d] +=mean[d];
			sd2[t][d] +=sd[d];
		  }

	  }
	  else
	  {
		  for (tt=t-window;tt<=t+window;tt++)
		  {
			   if(tt < 0)
			   {
				  //t_index=0;
				  if(frameNumber+tt<0)
					t_index =0;
				  else              
					t_index = frameNumber+tt;
			   }
			   else if(tt > frameNumber-1)
			   {
				  //t_index=frameNumber-1;
				  if(tt-frameNumber > frameNumber-1)
					 t_index = frameNumber-1;
				  else
					 t_index = tt-frameNumber;
			   }
			   else
				  t_index=tt;
				  
				  
			   for (d=0; d<OUT_DIM; ++d)
			   {
				  mean2[t][d] += Cep_Data[t_index][d];
				  sd2[t][d] += Cep_Data[t_index][d]*Cep_Data[t_index][d];
			   }
		  }
	  }

      
      for (d=0; d<OUT_DIM; ++d)
      {
          if (f_flag)
		  {
			mean2[t][d] /= (float)(frameNumber);
			sd2[t][d] = (float)sqrt(sd2[t][d]/(float)(frameNumber) - mean2[t][d]*mean2[t][d]);
		  }
		  else
		  {
			mean2[t][d] /= (float)(2.0f*window+1.0f);
			sd2[t][d] = (float)sqrt(sd2[t][d]/(float)(2.0f*window+1.0f) - mean2[t][d]*mean2[t][d]);
		  }
      }
   
      //printf("ok frame:%d mean2 sd2\n",t);
          
   }
   //printf("qtk! ok mean2 sd2!\n");
   
   //using dynamic mean & std
   for (t=0; t<frameNumber; ++t)
     for (d=0; d<OUT_DIM; ++d)
        Cep_Data[t][d] = (Cep_Data[t][d] - mean2[t][d])/sd2[t][d];   

//using whole sentence
/*
   for (d=0; d<OUT_DIM; ++d)
   {
      mean[d] = 0.0f;
      sd[d] = 0.0f;
   }

   for (t=0; t<frameNumber; ++t)
      for (d=0; d<OUT_DIM; ++d)
      {
         mean[d] += Cep_Data[t][d];
         sd[d]  += Cep_Data[t][d]*Cep_Data[t][d];
      }

   for (d=0; d<OUT_DIM; ++d)
   {
      mean[d] /= (float)frameNumber;
      sd[d] = (float)sqrt(sd[d]/(float)frameNumber - mean[d]*mean[d]);
   }


   for (t=0; t<frameNumber; ++t)
      for (d=0; d<OUT_DIM; ++d)
         Cep_Data[t][d] = (Cep_Data[t][d] - mean[d])/sd[d];
*/ 
        
   
   if(DO_AVG==1)
   {
      fea = (double *) malloc(sizeof(double)*frameNumber);
      for(d=0; d<OUT_DIM; d++)
      {
         for (t=ORDER_M; t<(frameNumber-ORDER_M); ++t)
         {
           fea[t] = 0.0f;
           for(i=(-ORDER_M); i<=ORDER_M; i++)
              fea[t] += Cep_Data[t+i][d];

      	    fea[t] /= (double)(2.0f*ORDER_M+1.0f);
         }

         //assign Cep_Data[t][d]
         for (t=ORDER_M+1; t<(frameNumber-ORDER_M); ++t)
            Cep_Data[t][d] = (float)fea[t];
      }
      if(fea)
      {
         free(fea);
         fea = NULL;
      }
   }
   else if(DO_AVG==2)
   {
      //do ARMA
      fea = (double *) malloc(sizeof(double)*1);
      for(d=0; d<OUT_DIM; d++)
      {
         for (t=ORDER_M; t<(frameNumber-ORDER_M); ++t)
         {
            *fea = 0.0f;
	    for(i=1; i<=ORDER_M; i++)
	    {
               (*fea) += Cep_Data[t+i][d];
               (*fea) += Cep_Data[t-i][d];
            }
            Cep_Data[t][d] += (float)(*fea);
      	    Cep_Data[t][d] /= (float)(2.0f*ORDER_M+1.0f);
         }
      }
      if(fea)
      {
         free(fea);
         fea = NULL;
      }

   }
}

void CMN_New(CEP_DATA *Cep_Data,int frameNumber)
{
	int	i,t;
	double cmean[CEP_DIM+1];
    
	//cepstrum mean normalize
	printf("%d\n",frameNumber);
	for (i=0; i<CEP_DIM+1 ;i++)
	{
		cmean[i] = 0.0f;
		for (t=0; t<frameNumber; t++)
		{
			cmean[i] += Cep_Data[t][i];
		}
		cmean[i] /= (float)frameNumber;
	}
   
   for (t=0; t<frameNumber; t++)
	{
		for (i=0; i<CEP_DIM+1; i++)
		{
			//printf("%f %f\n",Cep_Data[t][i],cmean[i]);
			Cep_Data[t][i] -= cmean[i];    
			//printf("%f %f\n",Cep_Data[t][i],cmean[i]);
			//getchar();
		}
	}

	return;
}
// output to file   //gb_cep_data[t][dim],gb_eng[t]   , gb_totalFrames
int OutputFeature(char *fileName)
{
	char   fname[512];
	int    i;
	FILE   *fp_C12, *fp_ENG;

	// C12 (EC_LTA_C12)
	sprintf(fname,"%s/%s.C12", OUT_C12_DIR, fileName);
	MakeDirectory(fname);
	fp_C12 = fopen(fname,"wb");
	if(fp_C12 == NULL)
	{
		printf("open C12 to write ERROR : %s\n", fname);
		return(0);
	}
	fwrite(&gb_totalFrames,sizeof(int),1,fp_C12);
	//write log10 eng
	for(i=0;i < gb_totalFrames;i++)
	{
		fwrite(gb_cep_data[i],sizeof(float)*CEP_DIM,1,fp_C12);
	}
	if(fp_C12)
		fclose(fp_C12);
	

	// ENG (EC_LTA_ENG)
	sprintf(fname,"%s/%s.ENG", OUT_EC_DIR, fileName);
	MakeDirectory(fname);
	fp_ENG = fopen(fname,"wb");
	if(fp_ENG == NULL)
	{
		printf("open ENG to write ERROR : %s\n", fname);
		return(0);
	}
	fwrite(&gb_totalFrames,sizeof(int),1,fp_ENG);
	//write log10 eng
	fwrite(gb_eng,sizeof(float),gb_totalFrames,fp_ENG);

	if(fp_ENG)
		fclose(fp_ENG);
	return(1);
}
void byteSwapShort(short *s)
 {
      char v;
      char *tmp = (char *)s ;
      v = *tmp;
      *tmp = *(tmp+1);
      *(tmp+1) = v;
   }
    void byteSwapInt(int *s) 
	{
      char v;
      char *tmp = (char *)s ;
      v = *(tmp+1);
      *(tmp+1) = *(tmp+2); *(tmp+2) = v;
      v = *(tmp);
      *(tmp) = *(tmp+3);
      *(tmp+3) = v;
   }
    void byteSwapArray(float *s, int d) 
	{
		int i;
      for(i=0;i<d;i++)
	  {
         char v;
         char *tmp = (char *)(s+i) ;
         v = *(tmp+1);
         *(tmp+1) = *(tmp+2); *(tmp+2) = v;
         v = *(tmp);
         *(tmp) = *(tmp+3);
         *(tmp+3) = v;
      }
   }
int OutputFeatureHTKFormat(char *fileName,char *extension)
{
	char   fname[512];
	int    i,t,d;
	FILE   *fp_fea;
	int nSamples, sampPeriod;
    short sampSize, parmKind;
    
float data[OUT_DIM];
	// C12 (EC_LTA_C12)
	sprintf(fname,"%s/%s.%s", OUT_C12_DIR, fileName, extension);
	MakeDirectory(fname);
	fp_fea = fopen(fname,"wb");
	
	nSamples=(int)gb_totalFrames;
		sampPeriod=HTK_HEADER_SAMPERIOD;
		//sampPeriod=100000; //for server case
		//sampPeriod=150000; //for embedded case
		sampSize=(int)sizeof(float)*OUT_DIM;
		parmKind=6;
		byteSwapShort(&sampSize); 
		byteSwapShort(&parmKind);
		byteSwapInt(&nSamples); 
		byteSwapInt(&sampPeriod);
		fwrite(&nSamples,   sizeof(int),  1, fp_fea);
		fwrite(&sampPeriod, sizeof(int),  1, fp_fea);
		fwrite(&sampSize,   sizeof(short), 1, fp_fea);
		fwrite(&parmKind,   sizeof(short), 1, fp_fea);
	for(t=0;t<gb_totalFrames;t++)
	{
		for(d=0;d<OUT_DIM;d++)
		{
			data[d]=out_data[t][d];
		}
		byteSwapArray(data,OUT_DIM);
		fwrite(data,sizeof(float),OUT_DIM,fp_fea);
	}
	if(fp_fea)
		fclose(fp_fea);
	
	return(1);
}
int AssignCepToOutData(void)
{
	int t,d;

	for(t=0;t<gb_totalFrames;t++)
	{
		for(d=0;d<CEP_DIM+1;d++)
		{
			out_data[t][d]=gb_cep_data[t][d];
			out_data[t][d+CEP_DIM+1]=delta_data[t][d];
			out_data[t][d+CEP_DIM*2+2]=delta2_data[t][d];
			//printf("%f %f %f\n",gb_cep_data[t][d],delta_data[t][d],delta2_data[t][d]);
			//getchar();
		}
	}/*
	for(t=0;t<gb_totalFrames;t++)
	{
		for(d=0;d<OUT_DIM;d++)
		{
			printf("t=%d D=%d %f\n",t,d,out_data[t][d]);
			getchar();
		}
	}
	*/
	return 1;
}
int main(int argc, char *argv[])
{
	FILE   *fp_lst;
	char   fileName[512], fname[512],extensionName[64];
	int t,d,MV_new;

	if(argc!=5)
	{
		printf("feature.exe <list file> <IN_WAV_DIR> <OUT_DIR> <CMN/MVNMA/NONE>\n");
		exit(0);
	}
	DO_CMN_OR_MVN=-1;
	if(strcmp(argv[4],"CMN")==0)
	{
		DO_CMN_OR_MVN=0;
	}
	else if(strcmp(argv[4],"MVNMA")==0)
	{
		DO_CMN_OR_MVN=1;
	}
	else if(strcmp(argv[4],"NONE")==0)
	{
		DO_CMN_OR_MVN=2;
	}
	if(DO_CMN_OR_MVN==-1)
	{
		printf("Normalization參數不正確！請輸入CMN或MVNMA\n");
		exit(0);
	}

	fp_lst = fopen(argv[1], "rt");
	sprintf(IN_WAV_DIR,"%s",argv[2]);
	sprintf(OUT_C12_DIR,"%s/",argv[3]);
	sprintf(OUT_EC_DIR,"%s/",argv[3]);

	if(fp_lst == NULL)
	{
		printf("cant open LST file : %s\n", argv[1]);exit(0);
	}
	ini_fea_para();
	while(fscanf(fp_lst, " %s\n",fileName)!=EOF)
	{
		sprintf(fname, "%s/%s", IN_WAV_DIR, fileName);

		MFCC_shell(fname);
		CEPLIFTER(gb_totalFrames);
		out_data=calloc(sizeof(OUT_DATA),gb_totalFrames);
		delta_data=calloc(sizeof(CEP_DATA),gb_totalFrames);
		delta2_data=calloc(sizeof(CEP_DATA),gb_totalFrames);
		if(DO_CMN_OR_MVN==0)
		{
			CMN_New(gb_cep_data,gb_totalFrames);
			strcpy(extensionName,"mfcATC0DAZC39");
		}
		getDeltaCep_New(gb_totalFrames,gb_cep_data,delta_data);
		getDeltaCep_New(gb_totalFrames,delta_data,delta2_data);
		AssignCepToOutData();
		if(DO_CMN_OR_MVN==1)
		{
			 //2013/12/2, fit small utters. to MV_WINDOW
			 if(gb_totalFrames<(MV_WINDOW*2+1))
				MV_new=(int)(gb_totalFrames/2); 
			 else
				MV_new=MV_WINDOW;
				
			 //MVN(out_data,gb_totalFrames);
			 //MVN2(out_data,gb_totalFrames,MV_WINDOW);
			 //MVN2(out_data,gb_totalFrames,MV_new); //2013/12/2, fit small utters. to MV_WINDOW
			 //MVN2_T1(out_data,gb_totalFrames,MV_new); //2013/12/3, padding tail frame: fixed by buff for tail(buff=MV_WINDOW*2+1)
			 MVN2_T2(out_data,gb_totalFrames,MV_new); //2013/12/3, padding tail frame: fixed by whole sentence for tail(buff=MV_WINDOW*2+1)
			 
			 if(DO_AVG==1)
			    strcpy(extensionName,"mfcATC0DAC39mva");
			 else if(DO_AVG==2)
			    strcpy(extensionName,"mfcATC0DAC39mvacjp_atc2");
		}
		else if(DO_CMN_OR_MVN==2)
		{
			strcpy(extensionName,"mfcATC0DAC39");
		}
		if(!OutputFeatureHTKFormat(fileName,extensionName))
		{
			printf("Error\n");
			getchar();			
		}		
		if(gb_cep_data)
			free(gb_cep_data);
		if(delta_data)
			free(delta_data);
		if(delta2_data)
			free(delta2_data);
		if(out_data)
			free(out_data);
	}

	if(fp_lst)
		fclose(fp_lst);
}
