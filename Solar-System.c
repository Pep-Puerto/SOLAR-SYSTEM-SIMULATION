/************************************************************************
All the programs in this site are under:

                    GNU GENERAL PUBLIC LICENSE
                    Version 3, 29 June 2007


**************************************************************************
Author:   Gac Genetic
email:    gac.genetic@gmail.com
*************************************************************************/

//gcc orbita.c -lm -lSDL2 -lSDL2main -lSDL2_ttf -o orbita
//You need arial.ttf on the same directory.

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <math.h>

#define true     1
#define false    0
#define N_OBJECTS       11

#define ZOOM_SPACE        0.005
#define ZOOM_TIME         0.005
#define VAR_ACCELERATION    100
#define STOP     0.000000000001

#define RADI_REP    10

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
#define SHIP        10

#define LEN_OBJECT_NAME  15

typedef struct object{
    char name[LEN_OBJECT_NAME];
    long double M;
    long double r;
    long double rep;
    long double x;
    long double y;
    long double vx;
    long double vy;
    long double ax;
    long double ay;
} t_object;
 
t_object object[N_OBJECTS];

double dt = 0.0000005;

//Screen dimension in pixels
double _W = 1800;
double _H =    900;
//Dimension of the real world in AU
double World_W = 3000;
double World_H    = 1500;

//Object centered
int center_ref_obj;

//Representation mode
int mode_rep;

int    _init_system_object();
void    _calc_orbits (int n_objects);
int    _object_intersection(long double radi, long double *x, long double *y, int object, int n_objects);

int    _center_object( int object, int n_objects );
int    _redim_object_rep_radius ( int n_objects, int factor);
int    _variar_radi_representacio_objectes();

int    _SDL_init( SDL_Window **window, SDL_Renderer **renderer, TTF_Font **font);
int    _SDL_end();
int    _SDL_draw_objects (SDL_Renderer* renderer, int n_objects);
int    _SDL_RenderDrawPlanet( SDL_Renderer* renderer, t_object object);
int    _SDL_RenderDrawInfo (SDL_Renderer * renderer,
                                 SDL_Surface * surface_object_ref, SDL_Texture * texture_object_ref,
                                 SDL_Surface * surface_world_dimensions, SDL_Texture * texture_world_dimensions,
                                 SDL_Surface * surface_time_dt, SDL_Texture * texture_time_dt,
                                 TTF_Font * font, SDL_Color color);
int    _SDL_manage_keyboard_events( SDL_Event event );                                

int main(int argc, char ** argv){
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

    _SDL_init( &window, &renderer, &font);
    _init_system_object();

    while (!quit){
          if (( SDL_PollEvent(&event) ) && (event.type == SDL_QUIT )) quit = true;
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
                SDL_RenderClear(renderer);
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
                _SDL_manage_keyboard_events(event);
                //Calculate the orbits   
                _calc_orbits(N_OBJECTS);
                //Draw the system
                _SDL_draw_objects(renderer, N_OBJECTS);
                //Show info                
                     _SDL_RenderDrawInfo (renderer,
                        surface_object_ref, texture_object_ref,
                        surface_world_dimensions, texture_world_dimensions,
                        surface_time_dt, texture_time_dt,
                        font, color);
                //Update screen
                SDL_RenderPresent(renderer);
    }
    _SDL_end(window, renderer, surface_object_ref, texture_object_ref, surface_world_dimensions, texture_world_dimensions, surface_time_dt, texture_time_dt);
    return 0;
}

int _SDL_init( SDL_Window **window, SDL_Renderer **renderer, TTF_Font **font){
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    *window = SDL_CreateWindow("Orbita", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, (int)_W, (int)_H, 0);
    *renderer = SDL_CreateRenderer(*window, -1, 0);
    *font = TTF_OpenFont("arial.ttf", 18);
}

int _SDL_end(    SDL_Window *window, SDL_Renderer *renderer,   
                    SDL_Surface *surface_object_ref, SDL_Texture *texture_object_ref,
                    SDL_Surface *surface_world_dimensions, SDL_Texture *texture_world_dimensions,
                    SDL_Surface *surface_time_dt, SDL_Texture *texture_time_dt){
                   
    SDL_DestroyTexture(texture_object_ref);
    SDL_FreeSurface(surface_object_ref);
    SDL_DestroyTexture(texture_world_dimensions);
    SDL_FreeSurface(surface_world_dimensions);
    SDL_DestroyTexture(texture_time_dt);
    SDL_FreeSurface(surface_time_dt);
   
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();                   
}

int _init_system_object(){
int i;
//Distances from the sun in AU units
//Radius in AU units
//M in reference to earth

    sprintf(object[SUN].name, "SUN");
    object[SUN].M        =    33294600;
    object[SUN].r        =    0.004652;
    object[SUN].rep      = object[SUN].r;
    object[SUN].x        =    0;
    object[SUN].y        =    0;
    object[SUN].vx       =    0;
    object[SUN].vy       =    0;
    object[SUN].ax       =    0;
    object[SUN].ay       =    0;

    sprintf(object[MERCURY].name, "MERCURY");
    object[MERCURY].M     =    6;
    object[MERCURY].r     =    0.0000163;
    object[MERCURY].rep   =    object[MERCURY].r;
    object[MERCURY].x     =    0.39;
    object[MERCURY].y     =    0;
    object[MERCURY].vx    =    0;
    object[MERCURY].vy    =    pow(object[SUN].M / object[MERCURY].x, 0.5);
    object[MERCURY].ax    =    0;
    object[MERCURY].ay    =    0;

    sprintf(object[VENUS].name, "VENUS");
    object[VENUS].M    =    82;
    object[VENUS].r    =    0.000040;
    object[VENUS].rep  =    object[VENUS].r;
    object[VENUS].x    =    0.72;
    object[VENUS].y    =    0;
    object[VENUS].vx   =    0;
    object[VENUS].vy   =    pow(object[SUN].M / object[VENUS].x, 0.5);
    object[VENUS].ax   =    0;
    object[VENUS].ay   =    0;

    sprintf(object[EARTH].name, "EARTH");
    object[EARTH].M    =    100;
    object[EARTH].r    =    0.000042;
    object[EARTH].rep  =    object[EARTH].r;
    object[EARTH].x    =    1;
    object[EARTH].y    =    0;
    object[EARTH].vx   =    0;
    object[EARTH].vy   =    pow(object[SUN].M / object[EARTH].x, 0.5);
    object[EARTH].ax   =    0;
    object[EARTH].ay   =    0;


    sprintf(object[MARS].name, "MARS");
    object[MARS].M     =    11;
    object[MARS].r     =    0.000022;
    object[MARS].rep   =    object[MARS].r;
    object[MARS].x     =    1.52;
    object[MARS].y     =    0;
    object[MARS].vx    =    0;
    object[MARS].vy    =    pow(object[SUN].M / object[MARS].x, 0.5);
    object[MARS].ax    =    0;
    object[MARS].ay    =    0;

    sprintf(object[JUPITER].name, "JUPITER");
    object[JUPITER].M     =    31800;
    object[JUPITER].r     =    0.000463;
    object[JUPITER].rep   =    object[JUPITER].r;
    object[JUPITER].x     =    5.20;
    object[JUPITER].y     =    0;
    object[JUPITER].vx    =    0;
    object[JUPITER].vy    =    pow(object[SUN].M / object[JUPITER].x, 0.5);
    object[JUPITER].ax    =    0;
    object[JUPITER].ay    =    0;

    sprintf(object[SATURN].name, "SATURN");
    object[SATURN].M      =    9500;
    object[SATURN].r      =    0.000386;
    object[SATURN].rep    =    object[SATURN].r;
    object[SATURN].x      =    9.54;
    object[SATURN].y      =    0;
    object[SATURN].vx     =    0;
    object[SATURN].vy     =    pow(object[SUN].M / object[SATURN].x, 0.5);
    object[SATURN].ax     =    0;
    object[SATURN].ay     =    0;

    sprintf(object[URANE].name, "URANE");
    object[URANE].M        =    800;
    object[URANE].r        =    0.000168;
    object[URANE].rep      =    object[URANE].r;
    object[URANE].x        =    14.60;
    object[URANE].y        =    0;
    object[URANE].vx       =    0;
    object[URANE].vy       =    pow(object[SUN].M / object[URANE].x, 0.5);
    object[URANE].ax       =    0;
    object[URANE].ay       =    0;

    sprintf(object[NEPTUNE].name, "NEPTUNE");
    object[NEPTUNE].M     =    17.20;
    object[NEPTUNE].r     =    0.000163;
    object[NEPTUNE].rep   =    object[NEPTUNE].r;
    object[NEPTUNE].x     =    300;
    object[NEPTUNE].y     =    10;
    object[NEPTUNE].vx    =    0;
    object[NEPTUNE].vy    =    pow(object[SUN].M / object[NEPTUNE].x, 0.5);
    object[NEPTUNE].ax    =    0;
    object[NEPTUNE].ay    =    0;

    sprintf(object[MOON].name, "MOON");
    object[MOON].M      = 1.23;
    object[MOON].r      = 0.000011;
    object[MOON].rep    = object[MOON].r;
    object[MOON].x      = 1.0026;
    object[MOON].y      = 0;
    object[MOON].vx     = 0;
    object[MOON].vy     = object[EARTH].vy + pow(object[EARTH].M / 0.0026, 0.5);
    object[MOON].ax     = 0;
    object[MOON].ay     = 0;
 
    sprintf(object[SHIP].name, "SHIP");
    object[SHIP].M     = 0.000000000000000000001;
    object[SHIP].r     = 0.000011;
    object[SHIP].rep   = object[SHIP].r;
    object[SHIP].x     = object[EARTH].x;
    object[SHIP].y     = object[EARTH].y - object[EARTH].r - object[SHIP].r - 0.0001;
    object[SHIP].vx    = 0;
    object[SHIP].vy    = pow(object[SUN].M / object[EARTH].x, 0.5);
    object[SHIP].ax    = 0;
    object[SHIP].ay    = 0;

 
  for ( i = 0; i < N_OBJECTS; i++ ) object[i].rep = RADI_REP * object[i].r;
  center_ref_obj = SUN;
  mode_rep = 0;

}

void _calc_orbits (int n) {
int i, j;
long double d, x, y, ax, ay;

//Calcula les velocitats de cada objecte
    for(i = 0; i < n-1; i++){
        for(j = i+1; j<n; j++){
            d = pow(pow(object[j].x-object[i].x,2) + pow(object[j].y-object[i].y,2),1.5);
            ax = (object[j].x-object[i].x)/d;
            ay = (object[j].y-object[i].y)/d;

            object[i].vx += object[j].M * ( ax + object[i].ax ) * dt;
            object[i].vy += object[j].M * ( ay + object[i].ay ) * dt;
            object[j].vx += object[i].M * (-ax + object[j].ax ) * dt;
            object[j].vy += object[i].M * (-ay + object[j].ay ) * dt;
        }
    }
//Calcula les noves posicions de cada objecte
    for(i = 0; i < n; i++){
        x = object[i].vx * dt + object[i].x;
        y = object[i].vy * dt + object[i].y;
        //Correcció posició si intersecció
        _object_intersection(object[i].r, &x, &y, i, n);
        object[i].x = x;
        object[i].y = y;
        }
}

int _object_intersection(long double radi, long double *x, long double *y, int obj, int n_objects){
int i;
long double d, v_x, v_y, u_x, u_y, v_modul;

    for ( i = 0; i < n_objects; i++){
        //Intersection object
        d = radi + object[i].r;
        if (     ( i != obj ) &&   
                ( pow(object[i].x - *x, 2) + pow(object[i].y - *y, 2) < d * d )
            )
        {
            v_x = object[i].x - object[obj].x;
            v_y = object[i].y - object[obj].y;
            v_modul = pow(pow( v_x, 2) + pow( v_y, 2), 0.5);
            u_x = v_x / v_modul;
            u_y = - v_y / v_modul;
            *x = object[i].x + ( object[i].r + radi ) * u_x;
            *y = object[i].y + ( object[i].r + radi ) * u_y;
        }
    }
}


int _SDL_draw_objects( SDL_Renderer* renderer, int n_objects ){
int i;
    _center_object(center_ref_obj, N_OBJECTS);
    for ( i = 0; i < n_objects; i++ )
        _SDL_RenderDrawPlanet( renderer, object[i]);
}

int _SDL_RenderDrawPlanet( SDL_Renderer* renderer, t_object object ){
int i, point_x, point_y;
int object_rep_x, object_rep_y, object_rep_r;
    //Color en funció de la masa o altre camp a t_object ...
    SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0x00, 0xFF );
    object_rep_x = (int) ((object.x + (World_W / 2)) * (_W / World_W));
    object_rep_y = (int) ((object.y + (World_H / 2)) * (_H / World_H));
    object_rep_r = (int) (object.r * (_H / World_H));
    //Paint a circle
    for (i = 0; i < 360; i++){
        point_x = object_rep_x + object_rep_r * cos((M_PI / 180 )*i);
        point_y = object_rep_y + object_rep_r * sin((M_PI / 180 )*i);
        SDL_RenderDrawPoint( renderer, point_x, point_y );
    }
}

int _SDL_RenderDrawInfo (SDL_Renderer * renderer,
                            SDL_Surface * surface_object_ref, SDL_Texture * texture_object_ref,
                            SDL_Surface * surface_world_dimensions, SDL_Texture * texture_world_dimensions,
                            SDL_Surface * surface_time_dt, SDL_Texture * texture_time_dt,
                            TTF_Font * font, SDL_Color color){
char object_ref[256];
char world_dimensions[256];
char time_dt[256];

int textW_object_ref = 0;
int textH_object_ref = 0;
int textW_world_dimensions = 0;
int textH_world_dimensions = 0;
int textW_time_dt = 0;
int textH_time_dt = 0;

    sprintf(object_ref, "%s %.5Lf %.5Lf | SHIP ax: %.4Lf ay: %.4Lf", object[center_ref_obj].name, object[center_ref_obj].r, object[center_ref_obj].rep, object[SHIP].ax, object[SHIP].ay );
    sprintf(world_dimensions, "W: %.3f H: %.3f AU", World_W, World_H );
    sprintf(time_dt, "dt: %.20f", dt );

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

int _center_object( int p, int n_objects ){
int i;
double dist_x, dist_y;

    dist_x = object[p].x;
    dist_y = object[p].y;
    for ( i = 0; i < n_objects; i++ ){
        object[i].x = object[i].x - dist_x;
        object[i].y = object[i].y - dist_y;
    }
}

int _variar_radi_representacio_objectes( ){
int i;
double tmp;
    //swap radio and radio_rep
    for ( i = 0; i < N_OBJECTS; i++ ){
        tmp = object[i].r;
        object[i].r = object[i].rep;
        object[i].rep = tmp;
    }
}

int _redim_object_rep_radius ( int n_objects, int factor){
int i;
    for ( i = 0; i < n_objects; i++ )
        object[i].r = object[i].r * factor;
}

int _SDL_manage_keyboard_events( SDL_Event event ){
    switch( event.type ){
        case SDL_KEYDOWN:    switch (event.key.keysym.sym){         
                                    case SDLK_LEFT:    dt = dt * ( 1 - ZOOM_TIME );
                                                            break;
                                    case SDLK_RIGHT:    dt = dt * ( 1 + ZOOM_TIME );
                                                            break;
                                    case SDLK_UP:        World_W = World_W * ( 1 + ZOOM_SPACE );
                                                            World_H = World_H * ( 1 + ZOOM_SPACE );
                                                            _center_object(center_ref_obj, N_OBJECTS);
                                                            break;
                                    case SDLK_DOWN:    World_W = World_W * ( 1 - ZOOM_SPACE );
                                                            World_H = World_H * ( 1 - ZOOM_SPACE );
                                                            _center_object(center_ref_obj, N_OBJECTS);
                                                            break;
                                    case SDLK_c:        _center_object(center_ref_obj, N_OBJECTS);
                                                            break;
                                    case SDLK_s:        dt = STOP;
                                                            break;
                                    case SDLK_r:        _variar_radi_representacio_objectes();
                                                            break;
                                    case SDLK_0:        center_ref_obj = SUN;
                                                            break;
                                    case SDLK_1:        center_ref_obj = MERCURY;
                                                            break;
                                    case SDLK_2:       center_ref_obj = VENUS;
                                                            break;
                                    case SDLK_3:        center_ref_obj = EARTH;
                                                            break;
                                    case SDLK_4:       center_ref_obj = MARS;
                                                            break;
                                    case SDLK_5:        center_ref_obj = JUPITER;
                                                            break;
                                    case SDLK_6:        center_ref_obj = SATURN;
                                                            break;
                                    case SDLK_7:        center_ref_obj = URANE;
                                                            break;
                                    case SDLK_8:        center_ref_obj = NEPTUNE;
                                                            break;
                                    case SDLK_9:        center_ref_obj = MOON;
                                                            break;
                                    case SDLK_n:        center_ref_obj = SHIP;
                                                            break;

                                    case SDLK_y:        object[SHIP].ay = object[SHIP].ay - VAR_ACCELERATION;
                                                            break;
                                    case SDLK_h:        object[SHIP].ay = object[SHIP].ay + VAR_ACCELERATION;
                                                            break;
                                    case SDLK_j:        object[SHIP].ay = 0;
                                                            break;
                                    case SDLK_t:        object[SHIP].ax = object[SHIP].ax - VAR_ACCELERATION;
                                                            break;
                                    case SDLK_u:        object[SHIP].ax = object[SHIP].ax + VAR_ACCELERATION;
                                                            break;
                                    case SDLK_g:        object[SHIP].ax = 0;
                                                            break;
                                    default:             break;
        }
      break;
      default: break;
    }

}
