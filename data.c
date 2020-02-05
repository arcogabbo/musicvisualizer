#include "data.h"

void myCallback(void* userData,Uint8* stream,int len)
{
    struct AudioData* audio=(struct AudioData*)userData;
    pthread_t th;
    struct wrapper wrap;
    
    if(audio->length==0)
        return;
    
    wrap.stream=stream;
    wrap.audio=audio;

    pthread_create(&th,NULL,visualizerOutput,(void*)&wrap);

    Uint32 length=(Uint32) len;
    length=(length > audio->length?audio->length:length); 

    sem_wait(&play);
    SDL_memcpy(stream,audio->position,length);
    
    audio->position+=length;
    audio->length-=length;     
}

double Get16bitAudioSample(Uint8* bytebuffer,SDL_AudioFormat format)
{
    Uint16 val= 0x0;

    if(SDL_AUDIO_ISLITTLEENDIAN(format))
        val=(uint16_t)bytebuffer[0] | ((uint16_t)bytebuffer[1] << 8);
    else
        val=((uint16_t)bytebuffer[0] << 8) | (uint16_t)bytebuffer[1];

    if(SDL_AUDIO_ISSIGNED(format))
        return ((int16_t)val)/32768.0;
    
    return val/65535.0;
}

void* visualizerOutput(void* arg)
{
    struct wrapper* wrap=(struct wrapper*)arg;
    double max[BARS];
    double multiplier;
    int window_size=2;
    int count=0;
    int sum;
    //double freq_bin[BARS+1] = {19.0,140.0,250.0,400.0,500.0,600.0,700.0,800.0,1000.0,1500.0,(double)SAMPLE_RATE/2};
    double freq_bin[BARS+1];
    double re,im;
    float CONSTANT=(float)NSAMPLES/WIDTH;
    float freq;
    double magnitude;
    int startx=0,starty=HEIGHT;
    struct HSV hsv;
    static int colorstart=0;

    //construct a range of frequencies based on NSAMPLES
    for(int i=0;i<BARS;i++)
    {
        max[i]=1.7E-308;
        freq_bin[i]=i*(SAMPLE_RATE/NSAMPLES)+i;
        //printf("%d:%lf\t",i,freq_bin[i]);
    }
    //printf("\n");
    freq_bin[BARS]=SAMPLE_RATE/2;

    for(int i=0;i<NSAMPLES;i++)
    {
        //getting values from stream and applying hann windowing function
        multiplier= 0.5*(1-cos(2*M_PI*i/NSAMPLES));
        
        wrap->audio->in[i][0]=Get16bitAudioSample(
                                    wrap->stream,
                                    wrap->audio->format)*multiplier;
        wrap->audio->in[i][1]=0.0;
        
        wrap->stream+=2;
    }

    //time domain visualizer (TESTING)
    if(MODE)
    {
        //
        //
        //INIT TIME DOMAIN MODE
        //
        //
        SDL_SetRenderDrawColor(wrap->audio->renderer,0,0,0,0);
        SDL_RenderClear(wrap->audio->renderer);
        hsv.h=( 2 + colorstart) % 360;
        hsv.s=hsv.v=1.0;
        conversion(hsv,wrap->audio->color);
        SDL_SetRenderDrawColor(wrap->audio->renderer,
                                    wrap->audio->color->r,
                                    wrap->audio->color->g,
                                    wrap->audio->color->b,
                                    255);
        //printf("1 entro ciclo\n");
        for(int i=0;i<NSAMPLES;i++)
        {
            wrap->audio->time_domain[i].x=i/CONSTANT;
            wrap->audio->time_domain[i].y=300-wrap->audio->in[i][0]*70;
        }
        //printf("2 esco ciclo\n");
        SDL_RenderDrawLines(wrap->audio->renderer,wrap->audio->time_domain,NSAMPLES);
        //printf("3 disegno\n");
        //
        //
        //ENDING TIME DOMAIN MODE
        //
        //
    }
    else
    {
        //
        //
        //INIT BARS MODE
        //
        //
        fftw_execute(wrap->audio->plan);
        
        //calculate magnitudes
        for(int j=0;j < NSAMPLES/2;j++)
        {
            re=wrap->audio->out[j][0];
            im=wrap->audio->out[j][1];

            magnitude=sqrt((re*re)+(im*im));
            
            freq=j*((double)SAMPLE_RATE/NSAMPLES);

            for(int i=0;i < BARS;i++)
                if((freq>freq_bin[i]) && (freq<=freq_bin[i+1]))
                    if(magnitude > max[i]) 
                        max[i]=magnitude;
        }
    
        //simple moving window average algorithm(hole scene smoother)
        /*for(int i=0;i<BARS;i++)
        {
            if(count<window_size)
                count++;
            sum=max[i];
            
            if(i+count < BARS)
                for(int j=0;j<count;j++)
                    sum+=max[i+j];
            else
                count-=window_size;
        
            if(i-count > 0)
                for(int j=0;j<count;j++)
                    sum+=max[i-j];

            max[i]=sum/(count*2+1);
        }*/

        SDL_SetRenderDrawColor(wrap->audio->renderer,0,0,0,0);
        SDL_RenderClear(wrap->audio->renderer);

        //SDL_SetRenderDrawColor(audio->renderer,255,0,0,255);
        //effects techniques
        //max[i]=f(max[i])
        //
        //normal:       f(x)=FIT_FACTOR*x
        //exponential:  f(x)=log(x*FIT_FACTOR2)*FIT_FACTOR
        //multiPeak:    f(x)=x/Peak[i]*FIT_FACTOR
        //maxPeak:      f(x)=x/Global_Peak*FIT_FACTOR
        //TODO: substitute this whole system with SDL_DrawRects for performance
        //int SDL_RenderDrawRects(SDL_Renderer* renderer, const SDL_Rect* rects,int count)
        for(int i=0;i<BARS;i++)
        {
            hsv.h=( (i * 2) + colorstart) % 360;
            hsv.s=hsv.v=1.0;
            conversion(hsv,wrap->audio->color);
            SDL_SetRenderDrawColor(wrap->audio->renderer,
                                    wrap->audio->color->r,
                                    wrap->audio->color->g,
                                    wrap->audio->color->b,
                                    255); 
            
            if(max[i]>2.0)
                max[i]=log(max[i]);

            for(int j=0;j<THICKNESS;j++)
                SDL_RenderDrawLine(wrap->audio->renderer,
                                    startx+(i*DISTANCE+j),
                                    starty,
                                    startx+(i*DISTANCE+j),
                                    starty-(FIT_FACTOR*max[i]));
        }
        //
        //
        //ENDING BARS MODE
        //
        //
    }
    colorstart+=2;
    SDL_RenderPresent(wrap->audio->renderer); 
    sem_post(&play);
} 
