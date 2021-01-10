//gcc orbita.c -lm -lSDL2 -lSDL2main -lSDL2_ttf -o orbita
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <math.h>

#define true      1
#define false     0
#define NP       10

#define ZOOM_SPACE   0.005

#define ZOOM_IN       0.5 //Ampliar
#define ZOOM_OUT        2 //Reduir

#define ZOOM_TIME    0.005
#define STOP         0.000000000001

#define SUN         0
#define MERCURY     1
#define VENUS       2
#define EARTH       3
#define MARS        4
#define JUPITER     5
#define SATURN      6
#define URANE       7
#define NEPTUNE     8
#define MOON        9

typedef struct planet{
    SDL_Rect rect;
    double M;
    double r;
    double vx;
    double vy;
    double x;
    double y;
} t_planet;
 
t_planet planet[NP];

double t = 0.0000005;
//Screen dimension in pixels
double _W;
double _H;
//Dimension of the real world in UA
double World_W;
double World_H;

//Object centered
int center_ref_obj;

int    _init_univers();
int    _init_system_planet();
void   _calc_orbits (int n_planets);

int _center_object( int planet, int n_planets );
int _redim_planet_rep_radius ( int n_planets, int factor);

int _draw_planets (SDL_Renderer* renderer, int n_planets);
int SDL_RenderDrawPlanet( SDL_Renderer* renderer, t_planet planet);
int SDL_RenderDrawInfo (SDL_Renderer * renderer,
                                 SDL_Surface * surface_object_ref, SDL_Texture * texture_object_ref,
                                 SDL_Surface * surface_world_dimensions, SDL_Texture * texture_world_dimensions,
                                 SDL_Surface * surface_time_dt, SDL_Texture * texture_time_dt,
                                 TTF_Font * font, SDL_Color color);

int main(int argc, char ** argv)
{
    int i, quit = false;
    SDL_Event event;
    SDL_Window * window;   
    SDL_Renderer * renderer;

    SDL_Surface * surface_object_ref;
    SDL_Texture * texture_object_ref;
    SDL_Surface * surface_world_dimensions;
    SDL_Texture * texture_world_dimensions;
    SDL_Surface * surface_time_dt;
    SDL_Texture * texture_time_dt;

    TTF_Font * font;   
    SDL_Color color = { 255, 255, 255 };

    _init_univers();   
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    window = SDL_CreateWindow("Orbita", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, (int)_W, (int)_H, 0);
    renderer = SDL_CreateRenderer(window, -1, 0);
    font = TTF_OpenFont("arial.ttf", 18);

    _init_system_planet();
   
    while (!quit)
    {
          if (( SDL_PollEvent(&event) ) && (event.type == SDL_QUIT )) quit = true;
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
                SDL_RenderClear(renderer);
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
                     switch( event.type ){
                            case SDL_KEYDOWN:    switch (event.key.keysym.sym){         
                                                        case SDLK_LEFT:    t = t * ( 1 - ZOOM_TIME );
                                                                           break;
                                                        case SDLK_RIGHT:   t = t * ( 1 + ZOOM_TIME );
                                                                           break;
                                                        case SDLK_UP:      World_W = World_W * ( 1 + ZOOM_SPACE );
                                                                           World_H = World_H * ( 1 + ZOOM_SPACE );
                                                                           //World_W = World_W * ZOOM_IN;
                                                                           //World_H = World_H * ZOOM_IN;
                                                                           _center_object(center_ref_obj, NP);
                                                                           break;
                                                        case SDLK_DOWN:    World_W = World_W * ( 1 - ZOOM_SPACE );
                                                                           World_H = World_H * ( 1 - ZOOM_SPACE );
                                                                           //World_W = World_W * ZOOM_OUT;
                                                                           //World_H = World_H * ZOOM_OUT;
                                                                           _center_object(center_ref_obj, NP);
                                                                           break;
                                                                          case SDLK_c:           _center_object(center_ref_obj, NP);
                                                                                                   break;
                                                                          case SDLK_s:           t = STOP;
                                                                                                   break;
                                                                          case SDLK_0:           center_ref_obj = SUN;
                                                                                                   break;
                                                                          case SDLK_1:           center_ref_obj = MERCURY;
                                                                                                   break;
                                                                          case SDLK_2:           center_ref_obj = VENUS;
                                                                                                   break;
                                                                          case SDLK_3:           center_ref_obj = EARTH;
                                                                                                   break;
                                                                          case SDLK_4:           center_ref_obj = MARS;
                                                                                                   break;
                                                                          case SDLK_5:           center_ref_obj = JUPITER;
                                                                                                   break;
                                                                          case SDLK_6:           center_ref_obj = SATURN;
                                                                                                   break;
                                                                          case SDLK_7:           center_ref_obj = URANE;
                                                                                                   break;
                                                                          case SDLK_8:           center_ref_obj = NEPTUNE;
                                                                                                   break;
                                                                          case SDLK_9:           center_ref_obj = MOON;
                                                                                                   break;
                                                                                                  
                                                      default: break;
                                                    }
                                                    break;
                             default: break;
                     }
                //Calculate the orbits   
                _calc_orbits(NP);
                //Draw the system
                _draw_planets(renderer, NP);
                //Show info                
        SDL_RenderDrawInfo (renderer,
                surface_object_ref, texture_object_ref,
                surface_world_dimensions, texture_world_dimensions,
                surface_time_dt, texture_time_dt,
                font, color);
                //Update screen
                SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture_object_ref);
    SDL_FreeSurface(surface_object_ref);
    SDL_DestroyTexture(texture_world_dimensions);
    SDL_FreeSurface(surface_world_dimensions);
    SDL_DestroyTexture(texture_time_dt);
    SDL_FreeSurface(surface_time_dt);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}

int _init_univers(){
//Screen dimension in pixels
_W = 1800;
_H = 900;
//Dimension of the real world in UA
World_W = 2000;
World_H = 1000;
}

int _init_system_planet(){
int k;
//Distances from the sun in UA units
//Radius in UA units
//M in reference to earth

  planet[SUN].vx    =    0;
  planet[SUN].vy    =    0;
  planet[SUN].M    =    33294600;
  planet[SUN].x    =    0;
  planet[SUN].y    =    0;
  planet[SUN].r    =     0.004652;

  planet[MERCURY].M      =    6;
  planet[MERCURY].x      =    0.39;
  planet[MERCURY].y      =    0;
  planet[MERCURY].r      =    0.0000163;
  planet[MERCURY].vx     =    0;
  planet[MERCURY].vy     =    pow(planet[SUN].M / planet[MERCURY].x, 0.5);

  planet[VENUS].M     =    82;
  planet[VENUS].x     =    0.72;
  planet[VENUS].y     =    0;
  planet[VENUS].r     =    0.000040;
  planet[VENUS].vx    =    0;
  planet[VENUS].vy    =    pow(planet[SUN].M / planet[VENUS].x, 0.5);


  planet[EARTH].M     =    100;
  planet[EARTH].x     =    1;
  planet[EARTH].y     =    0;
  planet[EARTH].r     =    0.000042;
  planet[EARTH].vx    =    0;
  planet[EARTH].vy    =    pow(planet[SUN].M / planet[EARTH].x, 0.5);

  planet[MARS].M      =    11;
  planet[MARS].x      =    1.52;
  planet[MARS].y      =    0;
  planet[MARS].r      =    0.000022;
  planet[MARS].vx     =    0;
  planet[MARS].vy     =    pow(planet[SUN].M / planet[MARS].x, 0.5);


  planet[JUPITER].M    =    31800;
  planet[JUPITER].x    =    5.20;
  planet[JUPITER].y    =    0;
  planet[JUPITER].r    =    0.000463;
  planet[JUPITER].vx   =    0;
  planet[JUPITER].vy   =    pow(planet[SUN].M / planet[JUPITER].x, 0.5);

  planet[SATURN].M    =    9500;
  planet[SATURN].x    =    9.54;
  planet[SATURN].y    =    0;
  planet[SATURN].r    =    0.000386;
  planet[SATURN].vx   =    0;
  planet[SATURN].vy   =    pow(planet[SUN].M / planet[SATURN].x, 0.5);

  planet[URANE].M     =    800;
  planet[URANE].x     =    14.60;
  planet[URANE].y     =    0;
  planet[URANE].r     =    0.000168;
  planet[URANE].vx    =    0;
  planet[URANE].vy    =    pow(planet[SUN].M / planet[URANE].x, 0.5);

  planet[NEPTUNE].M    =    17.20;
  planet[NEPTUNE].x    =    300;
  planet[NEPTUNE].y    =    10;
  planet[NEPTUNE].r    =    0.000163;
  planet[NEPTUNE].vx   =    0;
  planet[NEPTUNE].vy   =    pow(planet[SUN].M / planet[NEPTUNE].x, 0.5);

  planet[MOON].M      = 1.23;
  planet[MOON].x      = 1.0026;
  planet[MOON].y      = 0;
  planet[MOON].r      = 0.000011;
  planet[MOON].vx     = 0;
  planet[MOON].vy     = planet[EARTH].vy + pow(planet[EARTH].M / 0.0026, 0.5);
 
  int center_ref_obj = SUN;

}

void _calc_orbits (int n) {
int i, j;
double d,ax,ay;

//Calcula les velocitats de cada planeta
   for(i = 0; i < n-1; i++){
       for(j = i+1; j<n; j++){
         d = pow(pow(planet[j].x-planet[i].x,2) + pow(planet[j].y-planet[i].y,2),1.5);
         ax = (planet[j].x-planet[i].x)/d;
         ay = (planet[j].y-planet[i].y)/d;
 
         planet[i].vx += planet[j].M * ax * t;
         planet[i].vy += planet[j].M * ay * t;
         planet[j].vx += planet[i].M * (-ax) * t;
         planet[j].vy += planet[i].M * (-ay) * t;
        }
   }
//Calcula les noves posicions de cada planeta
   for(i = 0; i < n; i++){
      planet[i].x = planet[i].vx * t + planet[i].x;
      planet[i].y = planet[i].vy * t + planet[i].y;
   }
}

int _draw_planets( SDL_Renderer* renderer, int n_planets ){
int i;
 _center_object(center_ref_obj, NP);
 for ( i = 0; i < n_planets; i++ )
 SDL_RenderDrawPlanet( renderer, planet[i]);
}

int SDL_RenderDrawPlanet( SDL_Renderer* renderer, t_planet planet ){
int i, point_x, point_y;
int planet_rep_x, planet_rep_y, planet_rep_r;
    //Color en funció de la masa o altre camp a t_planet ...
    SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0x00, 0xFF );
    planet_rep_x = (int) ((planet.x + (World_W / 2)) * (_W / World_W));
    planet_rep_y = (int) ((planet.y + (World_H / 2)) * (_H / World_H));
    planet_rep_r = (int) (planet.r * (_W / World_W));
    //Paint a circle
    for (i = 0; i < 360; i++){
            point_x = planet_rep_x + planet_rep_r * cos((M_PI / 180 )*i);
            point_y = planet_rep_y + planet_rep_r * sin((M_PI / 180 )*i);
            SDL_RenderDrawPoint( renderer, point_x, point_y );
    }
}

int SDL_RenderDrawInfo (SDL_Renderer * renderer,
                                 SDL_Surface * surface_object_ref, SDL_Texture * texture_object_ref,
                                 SDL_Surface * surface_world_dimensions, SDL_Texture * texture_world_dimensions,
                                 SDL_Surface * surface_time_dt, SDL_Texture * texture_time_dt,
                                 TTF_Font * font, SDL_Color color){
char object_ref[40];
char world_dimensions[256];
char time_dt[256];

int textW_object_ref = 0;
int textH_object_ref = 0;
int textW_world_dimensions = 0;
int textH_world_dimensions = 0;
int textW_time_dt = 0;
int textH_time_dt = 0;

    sprintf(object_ref, "OBJREF: %d", center_ref_obj);
    sprintf(world_dimensions, "W: %.3f H: %.3f AU", World_W, World_H );
    sprintf(time_dt, "dt: %.12f", t );

   surface_object_ref = TTF_RenderText_Solid(font, object_ref, color);
   texture_object_ref = SDL_CreateTextureFromSurface(renderer, surface_object_ref);
       
    SDL_QueryTexture(texture_object_ref, NULL, NULL, &textW_object_ref, &textH_object_ref);
    SDL_FreeSurface(surface_object_ref);
    SDL_Rect rect_object_ref = { 0, 0, textW_object_ref, textH_object_ref };
    SDL_RenderCopy(renderer, texture_object_ref, NULL, &rect_object_ref);

   surface_world_dimensions = TTF_RenderText_Solid(font, world_dimensions, color);
   texture_world_dimensions = SDL_CreateTextureFromSurface(renderer, surface_world_dimensions);
       
    SDL_QueryTexture(texture_world_dimensions, NULL, NULL, &textW_world_dimensions, &textH_world_dimensions);
    SDL_FreeSurface(surface_world_dimensions);
    SDL_Rect rect_world_dimensions = { 0, 20, textW_world_dimensions, textH_world_dimensions };
    SDL_RenderCopy(renderer, texture_world_dimensions, NULL, &rect_world_dimensions);

   surface_time_dt = TTF_RenderText_Solid(font, time_dt, color);
   texture_time_dt = SDL_CreateTextureFromSurface(renderer, surface_time_dt);
       
    SDL_QueryTexture(texture_time_dt, NULL, NULL, &textW_time_dt, &textH_time_dt);
    SDL_FreeSurface(surface_time_dt);
    SDL_Rect rect_time_dt = { 0, 40, textW_time_dt, textH_time_dt };
    SDL_RenderCopy(renderer, texture_time_dt, NULL, &rect_time_dt);

    SDL_DestroyTexture(texture_object_ref);
    SDL_DestroyTexture(texture_world_dimensions);
    SDL_DestroyTexture(texture_time_dt);
}

int _center_object( int p, int n_planets ){
int i;
double dist_x, dist_y;
dist_x = planet[p].x;
dist_y = planet[p].y;
for ( i = 0; i < n_planets; i++ ){
     planet[i].x = planet[i].x - dist_x;
     planet[i].y = planet[i].y - dist_y;
}
}

int _redim_planet_rep_radius ( int n_planets, int factor){
int i;
for ( i = 0; i < n_planets; i++ )    planet[i].r = planet[i].r * factor;
}
