#version 330 core

// Ouput data
out vec4 color;

// constants
const int MAX_PARTICLES = 75;
const float ISO = 0.01f;								// the ISO threshold
const int K = 3;										// intensity of the scalar field

// uniforms
uniform vec4 particle_positions_camera[MAX_PARTICLES];	// particle positions in camera space
uniform mat4 M;											// model matrix
uniform mat4 V;											// view matrix
uniform mat4 P;											// projection matrix
uniform mat4 P_inv;										// inverse projection matrix
uniform vec4 viewport;									// the viewport

//----------------
// Lighting stuff
//----------------

// fixed point light properties
uniform vec3 light_pos_world;
const vec3 Ls = vec3(0.8f, 0.6f, 1.0f); // specular colour
const vec3 Ld = vec3(1.0f, 0.5f, 0.0f); // diffuse colour
const vec3 La = vec3(0.1f, 0.1f, 0.1f); // ambient color

// surface reflectance
vec3 Ks = vec3 (1.0f, 1.0f, 1.0f); // fully reflect specular light
vec3 Kd = vec3 (1.0f, 0.8f, 1.0f); // diffuse surface reflectance
vec3 Ka = vec3 (1.0f, 1.0f, 1.0f); // fully reflect ambient light

vec4 ComputeBlinnPhong(vec3 P, vec3 N)
{ 
	float rim_scalar = 3.5f;
	float shinyness = 150.0f;
	vec3 light_pos_camera = vec3 (V * vec4 (light_pos_world, 1.0f));

	vec3 L = normalize(light_pos_camera - P);
	vec3 V = normalize(-P);
	vec3 H = normalize(L + V);
	float NdotH = dot(N,H); 
	float NdotL = dot(N,L);
	float VdotH = dot(V,H); 
	float NdotV = dot(N,V);

	vec3 ambient = Ka * La;

	float diff_light = max(NdotL, 0.0f);
	vec3 diffuse = Kd * Ld * diff_light;

	float spec_light = pow(max(NdotH, 0.0f), shinyness);
	vec3 specular = Ks * Ls * spec_light;

	float attenuation = 10.0f * (1.0f / distance(light_pos_camera.xyz,P.xyz));

	vec3 rim = vec3(1.0f - max(0.0f, NdotV));
	rim *= rim;
	rim *= max(0.0f, NdotL) * Ls.rgb; 

	vec4 result = vec4(ambient + diffuse + specular + rim * rim_scalar * attenuation, 1.0f); 
	return result;
}

float ComputeRadialBasisFunction(vec3 x, vec3 xi)
{
	float dist = distance(x,xi);
	float exponent = -K * dist;
	float s = exp(exponent);
	return s;
}

float SumUpScalarFields(vec3 xi)
{
	float sum = 0.0f;

	for(int i=0; i<MAX_PARTICLES; ++i)
	{
		sum += ComputeRadialBasisFunction(particle_positions_camera[i].xyz, xi);
	}

	return sum;
}

// returns fragment position in world space
vec4 FragmentToCamSpace()
{
	vec4 ndc_pos;
	ndc_pos.xy = ((2.0f * gl_FragCoord.xy) - (2.0f * viewport.xy)) / (viewport.zw) - 1;
	ndc_pos.z = (2.0f * gl_FragCoord.z - gl_DepthRange.near - gl_DepthRange.far) / (gl_DepthRange.far - gl_DepthRange.near);
	ndc_pos.w = 1.0f;

	vec4 clip_pos = ndc_pos / gl_FragCoord.w;
	vec4 result = P_inv * clip_pos;

	return result;
}

// calculates the direction in which the scalar field gets stronger (so the negative normal of the fluid)
vec3 grads(vec3 p)
{
	float delta = .1f;

	// calculate x
	vec3 pXPlusDelta = p;
	pXPlusDelta.x += delta;

	vec3 pXMinusDelta = p;
	pXMinusDelta.x -= delta;
	float x = ( SumUpScalarFields(pXPlusDelta) - SumUpScalarFields(pXMinusDelta) ) / (2.0f*delta);

	// calculate y
	vec3 pYPlusDelta = p;
	pYPlusDelta.y += delta;

	vec3 pYMinusDelta = p;
	pYMinusDelta.y -= delta;
	float y = ( SumUpScalarFields(pYPlusDelta) - SumUpScalarFields(pYMinusDelta) ) / (2.0f*delta);

	// calculate z
	vec3 pZPlusDelta = p;
	pZPlusDelta.z += delta;

	vec3 pZMinusDelta = p;
	pZMinusDelta.z -= delta;
	float z = ( SumUpScalarFields(pZPlusDelta) - SumUpScalarFields(pZMinusDelta) ) / (2.0f*delta);

	return vec3(x,y,z);
}

void main(){

	vec4 frag_pos_camera = FragmentToCamSpace();

	// raytrace and look for intersection with scalar field
	// set origin some units in front of the billboard
	vec3 origin = frag_pos_camera.xyz - 2.0f * normalize(frag_pos_camera.xyz);
	vec3 ray_dir = normalize(frag_pos_camera.xyz - origin);
	vec3 ray_pos;
	float step = 1.0f;		// how many times the direction should be added to the rayposition each iteration
	float max_step = 10.0f;	// how far do we want to check the raycast?
	float value = 0.0f;		// ISO-value of the rough check
	float new_value = 0.0f;	// ISO-value of the binary search
	
	// raytracing starts here
	for(int i=0; i<max_step; ++i)
	{
		// rough raytrace to check if there is an intersection at all
		ray_pos = origin+i*step*ray_dir;
		value = SumUpScalarFields(ray_pos);

		if(value>=ISO)
		{
			// intersection found
			// bisection search to find more exact point of intersection
			step *= .5f;
			ray_pos -= step*ray_dir;

			for(int j=0; j<10; ++j)
			{
				step *= .5f;
				new_value = SumUpScalarFields(ray_pos);
				
				if(new_value<ISO)
				{
					ray_pos += step*ray_dir;
				}
				else if(new_value>ISO)
				{
					ray_pos -= step*ray_dir;
				}
			}

			vec3 normal = -normalize(grads(ray_pos));	// negate because gradient gets stronger closer to the particle
			color = ComputeBlinnPhong(ray_pos, normal);

			return;
		}
	}

	// no intersection of the scalar field -> discard fragment
	color = vec4(1.0f, 0.0f, 0.0f, 0.0f);
}
