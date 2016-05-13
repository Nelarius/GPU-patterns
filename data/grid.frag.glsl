#version 430 core

in float v_scalar;
out vec4 fragColor;

vec4 shortRainbow( float scalar ) {	//scalar must be normalized
    scalar /= 5.0;
    scalar = clamp(scalar, 0.0, 1.0);
    vec3 color = vec3( 0.0, 0.0, 0.0 );
    if ( scalar >= 0.0 && scalar < 0.25 ) {
        color.g = scalar * 4.0;	                    //green varies linearly between [0,1]
        color.b = 1.0;			                    //blue is maxed out
    } else if ( scalar >= 0.25 && scalar < 0.35 ) {
    	color.g = 1.0;			                    //green is maxed out
    	color.b = 1.0 - 10.0 * ( scalar - 0.25 );	//blue varies between [1,0]
    } else if ( scalar >= 0.35 && scalar < 0.75 ) {
    	color.r = ( scalar - 0.35 ) * 2.5;	        //red varies between [0,1]
    	color.g = 1.0;					            //green is maxed out
    } else if ( scalar >= 0.75 && scalar <= 1.0 ) {
    	color.r = 1.0;	                            //red is maxed out
    	color.g = 1.0 - 4.0 * ( scalar - 0.75 );	//green varies between [1,0]
    }
    return vec4( color, 1.0 );
}

vec4 longRainbow( float scalar ) {
    vec3 color = vec3( 0.0, 0.0, 0.0 );
    scalar = clamp(scalar, 0.0, 1.0);
    if ( scalar >= 0.0 && scalar < 0.1666 ) {
        color.g = scalar * 6.0;
        color.b = 1.0;
    } else if ( scalar >= 0.1666 && scalar < 0.3333 ) {
        color.g = 1.0;
        color.b = 1.0 - 6.0 * (scalar - 0.1666);
    } else if ( scalar >= 0.3333 && scalar < 0.5000 ) {
        color.r = 6.0 * (scalar - 0.3333);
        color.g = 1.0;
    } else if ( scalar >= 0.5000 && scalar < 0.6666 ) {
        color.r = 1.0;
        color.g = 1.0 - 6.0 * (scalar - 0.5000);
    } else if ( scalar >= 0.6666 && scalar < 0.8333 ) {
    color.b = 6.0*(scalar - 0.6666);
    color.r = 1.0;
    } else if ( scalar >= 0.8333 && scalar <= 1.0 ) {
    color.b = 1.0;
    color.r = 1.0 - 6.0 * (scalar - 0.8333);
    }
    return vec4( color, 1.0 );
}

void main() {
    fragColor = shortRainbow(v_scalar);
}
