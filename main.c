#include "data.h"

int main(int argc,char* argv[])
{
    //timing variables
    Uint32 start_time=0;
    Uint32 end_time=0;
    Uint32 delta=0;
    short fps=60;
    short time_per_frame= 15; //milliseconds

    //options arguments
    int option_index=0;
    char* file_stream=NULL;
    //boolean like variable to pause or resume the program
    int running=1;
    int playing=0;
    MODE=0;

    //SDL DECLARATIONS
    SDL_AudioSpec wav_spec;
    SDL_AudioSpec obtained;
    SDL_AudioDeviceID device;
    SDL_Window *win;
    SDL_Event event;
    Uint8 *wav_buffer;
    Uint32 wav_length;
    struct AudioData* audio=(struct AudioData*)malloc(sizeof(struct AudioData));

    sem_init(&play,0,0);
    
    NSAMPLES=2048;
    //getting option from terminal
    while((option_index=getopt(argc,argv,"s:f:")) != -1)
    {
        switch(option_index)
        {
            case 's':
                NSAMPLES=atoi(optarg);
                break;
            case 'f':
                file_stream=optarg;
                break;
            default:
                printf("Option error\n");
                exit(1);
        }
    }

    if(file_stream==NULL)
    {
        printf("You need to specify a file\n");
        exit(1);
    }
    //memory allocation for the arrays
    audio->in=(fftw_complex*)fftw_malloc(sizeof(fftw_complex)*NSAMPLES);
    audio->out=(fftw_complex*)fftw_malloc(sizeof(fftw_complex)*NSAMPLES);
    audio->color=(struct RGB*)malloc(sizeof(struct RGB));
    audio->time_domain=(SDL_Point*)malloc(sizeof(SDL_Point)*NSAMPLES);


    //planning the forward discrete fourier transformation in 1 dimension
    //FFTW_FORWARD is a constant that select the forward method and
    //FFTW_ESTIMATE is an estimate of the time it should take(to verify)
    audio->plan=fftw_plan_dft_1d(NSAMPLES,
                                audio->in,
                                audio->out,
                                FFTW_FORWARD,FFTW_MEASURE);

    //initialize video and audio
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)==-1)
        exit(1);

    win=SDL_CreateWindow("Hello",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,WIDTH,HEIGHT,SDL_WINDOW_OPENGL);
    audio->renderer=SDL_CreateRenderer(win,-1,SDL_RENDERER_ACCELERATED);
    
    if(SDL_LoadWAV(file_stream,&wav_spec,&wav_buffer,&wav_length) == NULL)
    {
        fprintf(stderr, "impossibile aprire il file wav: %s\n",SDL_GetError());
        exit(1);
    }

    audio->position=wav_buffer;
    audio->length=wav_length;
    wav_spec.freq=SAMPLE_RATE;
    wav_spec.channels=2;
    wav_spec.samples=NSAMPLES;
    wav_spec.callback=myCallback;
    wav_spec.userdata=audio;
    wav_spec.format=AUDIO_S16;
    audio->format=wav_spec.format;
    
    device=SDL_OpenAudioDevice(NULL,0,&wav_spec,&obtained,SDL_AUDIO_ALLOW_FORMAT_CHANGE);

    if(device==0)
    {
        printf("Errore nell'audio device: %s\n",SDL_GetError());
        exit(1);
    }

    SDL_PauseAudioDevice(device,playing);

    while(running)
    {
        //time in milliseconds since the program started
        if(!start_time)
            start_time=SDL_GetTicks();
        else
            delta= end_time - start_time;

        //if the delta is less than 16ms, then delay
        if(delta < time_per_frame)
            SDL_Delay(time_per_frame - delta);
        
        //else if the delta is > than 16ms, calculate fps
        if(delta > time_per_frame)
            fps=1000/delta;

        //printf("FPS:%i\n",fps);

        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
                running=0;
            
            if(event.type == SDL_KEYDOWN)
            {
                switch(event.key.keysym.sym)
                {
                    case SDLK_q:
                        running=0;
                        break;
                    case SDLK_p:
                        playing=!playing;
                        SDL_PauseAudioDevice(device,playing);
                        break;
                    case SDLK_m:
                        MODE=!MODE;
                        break;
                }
            }
        }
        

        start_time=end_time;
        end_time=SDL_GetTicks();
   }    

    //fttw cleanup
    fftw_destroy_plan(audio->plan);
    fftw_cleanup();
    fftw_free(audio->in);
    fftw_free(audio->out);
    
    //SDL cleanup
    SDL_FreeWAV(wav_buffer);
    SDL_CloseAudioDevice(device);
    SDL_DestroyRenderer(audio->renderer);
    SDL_DestroyWindow(win);
    free(audio->color);
    win=NULL;
    audio->renderer=NULL;
    audio->color=NULL;

    SDL_Quit();

    return 0;
}
