#ifndef _INTERP2D_
#define _INTERP2D_

#ifdef __CUDACC__
#define CUDA_CALLABLE_MEMBER __host__ __device__
#define CUDA_CALLABLE_MEMBER_DEVICE __device__
#else
#define CUDA_CALLABLE_MEMBER
#define CUDA_CALLABLE_MEMBER_DEVICE
#endif

#include <thrust/device_vector.h>
#include <vector>
#include <math.h>
using namespace std;

CUDA_CALLABLE_MEMBER

float interp2dCombined ( float x, float y, float z,int nx, int nz,
//#ifdef __CUDACC__
float* gridx,float* gridz,float* data ) {
/*#else        
std::vector<float>* gridxp,std::vector<float>* gridzp,std::vector<float>* datap ) {
    std::vector<float>& data = *datap;
    std::vector<float>& gridx = *gridxp;
    std::vector<float>& gridz = *gridzp;
#endif
*/
    float fxz = 0.0;
    if(nx*nz == 1)
    {
        fxz = data[0];
    }
    else{
#if USECYLSYMM > 0
    float dim1 = sqrt(x*x + y*y);
#else
    float dim1 = x;
#endif    
    float d_dim1 = gridx[1] - gridx[0];
    float dz = gridz[1] - gridz[0];
    int i = floor((dim1 - gridx[0])/d_dim1);//addition of 0.5 finds nearest gridpoint
    int j = floor((z - gridz[0])/dz);

    float interp_value = data[i + j*nx];
    //std::cout << "interp r and z pos "<< dim1 << " " << z <<std::endl;
    //std::cout << "interp i and j pos "<< i << " " << j <<std::endl;
    //std::cout << "nearest value "<< interp_value <<std::endl;

    float fx_z1 = ((gridx[i+1]-dim1)*data[i+j*nx] + (dim1 - gridx[i])*data[i+1+j*nx])/d_dim1;
    //std::cout << "Interp point 2 " <<std::endl;
    float fx_z2 = ((gridx[i+1]-dim1)*data[i+(j+1)*nx] + (dim1 - gridx[i])*data[i+1+(j+1)*nx])/d_dim1; 
    fxz = ((gridz[j+1]-z)*fx_z1+(z - gridz[j])*fx_z2)/dz;
    }

    //std::cout << "Interp point 3 " <<std::endl;
    return fxz;
}

CUDA_CALLABLE_MEMBER
void interp2dVector (float* field, float x, float y, float z,int nx, int nz,
float* gridx,float* gridz,float* datar, float* dataz, float* datat ) {

   float Ar = interp2dCombined(x,y,z,nx,nz,gridx,gridz, datar);
   float At = interp2dCombined(x,y,z,nx,nz,gridx,gridz, datat);
   field[2] = interp2dCombined(x,y,z,nx,nz,gridx,gridz, dataz);
   //std::cout << "r component " << Ar << std::endl;
   //std::cout << "t component " << At << std::endl;
   //std::cout << "z component " << field[2] << std::endl;
#if USECYLSYMM > 0
            //if cylindrical geometry
            /*
            float theta = atan(y/x);
            if(x < 0.0)
            {
                if(y > 0.0)
                {
                    theta = theta + 3.14159265358979;
                }
                else
                {
                    theta = sqrt(theta*theta) + 3.14159265358979;
                }
            }
            */
            float theta = atan2(y,x);   
            field[0] = cos(theta)*Ar - sin(theta)*At;
            field[1] = sin(theta)*Ar + cos(theta)*At;
#else
            field[0] = Ar;
            field[1] = At;
#endif

}
CUDA_CALLABLE_MEMBER_DEVICE

float interp2dcuda ( float x, float y, float z,int nx, int nz, float* gridx,
        float* gridz,float* data ) {

#if USECYLSYMM > 0
    float dim1 = sqrt(x*x + y*y);
#else
    float dim1 = x;
#endif    
    float d_dim1 = gridx[1] - gridx[0];
    float dz = gridz[1] - gridz[0];
    int i = floor((dim1 - gridx[0])/d_dim1 );//addition of 0.5 finds nearest gridpoint
    int j = floor((z - gridz[0])/dz);
    float interp_value = data[i + j*nz];
    float fx_z1 = ((gridx[i+1]-dim1)*data[i+j*nx] + (dim1 - gridx[i])*data[i+1+j*nx])/d_dim1;
    float fx_z2 = ((gridx[i+1]-dim1)*data[i+(j+1)*nx] + (dim1 - gridx[i])*data[i+1+(j+1)*nx])/d_dim1; 
    float fxz = ((gridz[j+1]-z)*fx_z1+(z - gridz[j])*fx_z2)/dz;
    return fxz;
}

float interp1dUnstructured(float samplePoint,int nx, float max_x, float* data)
{
    int done = 0;
    int low_index = 0;
    float interpolated_value = 0.0;

    for(int i=0;i<nx;i++)
    {
        if(done == 0)
        {
            if(samplePoint < data[i])
            {
                done = 1;
                low_index = i-1;
            }   
        }
    }
//    std::cout << "Sample point and low index" << samplePoint << " " <<
//        low_index << std::endl;
    interpolated_value =
        ((data[low_index+1] - samplePoint)*low_index*max_x/nx
        + (samplePoint - data[low_index])*(low_index+1)*max_x/nx)/(data[low_index+1]- data[low_index]);
    return interpolated_value;
}
#endif

