#include "MFIOperator.h"
#include "hoNDArray_utils.h"
#include "cuNDArray_utils.h"
#include "hoArmadillo.h"
#include "cuNDArray_elemwise.h"
#include "cuNDArray_operators.h"
#include "sense_utilities.h"
#include "b1_map.h"

namespace Gadgetron{

//constructor
MFIOperator::MFIOperator()
{
  is_prepared = false;
}

//Constructor that calls prepare
MFIOperator::MFIOperator(boost::shared_ptr<cuNFFT_plan<float,2>> plan_, cuNDArray<floatd2>& gpu_traj, std::vector<size_t> &data_d,std::vector<size_t> &image_d, uint64d2 &image_d_os, double &st,float dTE)
{
<<<<<<< HEAD
    this->prepare(plan_, gpu_traj, data_d, image_d, image_d_os, st, dTE);
=======
    this->Prepare(plan_, gpu_traj, data_d, image_d, image_d_os, st, dTE);
>>>>>>> 78428447f8c051b458fbf61261aaddb589182734
}

MFIOperator::~MFIOperator()
{

}

<<<<<<< HEAD
bool MFIOperator::prepare(boost::shared_ptr<cuNFFT_plan<float, 2>> plan_, cuNDArray<floatd2>& gpu_traj,
    std::vector<size_t>& data_d, std::vector<size_t>& image_d, uint64d2& image_d_os, double& st, float dTE)
=======
bool MFIOperator::Prepare(boost::shared_ptr<cuNFFT_plan<float,2>> plan_, cuNDArray<floatd2>& gpu_traj, std::vector<size_t> &data_d,std::vector<size_t> &image_d, uint64d2 &image_d_os, double &st,float dTE)
>>>>>>> 78428447f8c051b458fbf61261aaddb589182734
{

  //Setup MFIoperator instance if not already prepared
  //Otherwise check that nothing has changed
  //If changed, run prepare again, otherwise return true
  if(!is_prepared){

    data_dims = data_d;
    image_dims = image_d;

    size_t R0 = data_d[0];

    sample_time = st;
    delTE = dTE;
    fmax = std::ceil(1.2/(2.*delTE));
    L = std::ceil(2.5*fmax*R0*sample_time);
    if(L%2 == 0){ L++; }
    MFI_C.create( fmax*2+1 , L );

    //nfft_plan_ = NFFT<cuNDArray,float,2>::make_plan( from_std_vector<size_t,2>(image_d), image_d_os, 8.5 );
    //nfft_plan_->preprocess(&gpu_traj, NFFT_prep_mode::ALL);
    nfft_plan_ = plan_;

<<<<<<< HEAD
    this->calc_MFI_coeff();
    this->calc_kspace_filter(image_dims);
    this->calc_phase_mask();
=======
    this->Calc_MFI_Coeff();
    this->Calc_Kspace_Filter(image_dims);
    this->Calc_Phase_Mask();
>>>>>>> 78428447f8c051b458fbf61261aaddb589182734

    is_prepared = true;
    return is_prepared;

  }
  else{

    if(data_dims != data_d || image_dims != image_d){
      is_prepared = false;
<<<<<<< HEAD
      this->prepare(plan_, gpu_traj, data_d, image_d, image_d_os, st, dTE);
=======
      this->Prepare(plan_, gpu_traj, data_d, image_d, image_d_os, st, dTE);
>>>>>>> 78428447f8c051b458fbf61261aaddb589182734
      return is_prepared;
    }
    else{
      return is_prepared;
    }
  }

}

<<<<<<< HEAD
void MFIOperator::calc_MFI_coeff()
=======
void MFIOperator::Calc_MFI_Coeff()
>>>>>>> 78428447f8c051b458fbf61261aaddb589182734
{

  //Compute MFI coefficients if not they do not already exist
    float R0 = data_dims[0];

    arma::cx_fmat demod( R0 , L );
    arma::cx_fmat b( R0, fmax*2+1);
    arma::cx_fmat x( L, fmax*2+1 );
    std::complex<float> om(0.0,2.*M_PI);

    //Setup and solve least-squares probelm
    int j;
    int i;
    float f;
<<<<<<< HEAD
    #pragma omp parallel for private(f,i,j) shared(fmax, R0, L, om, demod)
=======
    #ifdef USE_OMP
    #pragma omp parallel for private(f,i,j) shared(fmax, R0, L, om, demod)
    #endif
>>>>>>> 78428447f8c051b458fbf61261aaddb589182734
    for(j = 0; j<L; j++){
      f = -fmax+float(j)*fmax*2./float(L-1);
      for(i = 0; i < R0; i++) {
        demod(i,j) = exp(om*(float)i*(float)sample_time*f);
      }
    }
<<<<<<< HEAD

    #pragma omp parallel for private(f,i,j) shared(fmax, R0, L, om, demod, b, x)
=======
    #ifdef USE_OMP
    #pragma omp parallel for private(f,i,j) shared(fmax, R0, L, om, demod, b, x)
    #endif
>>>>>>> 78428447f8c051b458fbf61261aaddb589182734
    for(j = 0; j<fmax*2+1; j++){
      f = -fmax+j;
      for(i = 0; i < R0; i++) {
        b(i,j) = exp(om*(float)i*(float)sample_time*f);
      }
      x.col(j) = arma::solve(demod, b.col(j));
<<<<<<< HEAD
      std::copy_n(x.colptr(j),L, MFI_C.get_data_ptr()+j*L);
=======
      memcpy(MFI_C.get_data_ptr()+j*L, x.colptr(j), L*sizeof(std::complex<float>));
>>>>>>> 78428447f8c051b458fbf61261aaddb589182734
    }
    //write_nd_array(&MFI_C,"MFI.cplx");
}

<<<<<<< HEAD
void MFIOperator::calc_phase_mask()
=======
void MFIOperator::Calc_Phase_Mask()
>>>>>>> 78428447f8c051b458fbf61261aaddb589182734
{
  //Phase mask for fast demodulation in gridded domain (See MFI paper)
  hoNDArray<std::complex<float>> tau(data_dims[0],data_dims[1],data_dims[2]);
  hoNDArray<complext<float>> phase_mask(&image_dims);
  cuNDArray<complext<float>> ch_images(&image_dims);
  float f_step = fmax/((L-1)/2.);
  size_t R0 = data_dims[0];

  for(int r = 0; r < data_dims[0]*data_dims[1]*data_dims[2]; r++) {
    tau[r] = std::complex<float>(sample_time*float(r%R0),0.0);
  }
  cuNDArray<complext<float>> gpu_data((hoNDArray<float_complext>*)&tau);
  nfft_plan_->compute( &gpu_data,ch_images, nullptr, NFFT_comp_mode::BACKWARDS_NC2C );
  nfft_plan_->fft(ch_images, NFFT_fft_mode::FORWARDS);

  auto time_grid = *(ch_images.to_host());
  for (int i = 0; i < time_grid.get_number_of_elements(); i++) {
    phase_mask[i] = exp(complext<float>(0.0,2.*M_PI*f_step*(abs(time_grid[i]))));
  }
  cu_phase_mask.create(*phase_mask.get_dimensions());
  cu_phase_mask = phase_mask;
  //write_nd_array(&phase_mask, "phase_mask.cplx");
}

<<<<<<< HEAD
void MFIOperator::calc_kspace_filter(std::vector<size_t>& matrix_size)
=======
void MFIOperator::Calc_Kspace_Filter(std::vector<size_t> &matrix_size)
>>>>>>> 78428447f8c051b458fbf61261aaddb589182734
{
  //Setup circular k-space filter to get rid of spurious high-frequency data from forward and backward FFTs (Similar to CG-sense)
  hoNDArray<std::complex<float>> kspace_filter(&matrix_size);
  float kx;
  float ky;
  for(int x = 0; x < matrix_size[0]; x++){
    kx = (-1+x*2.0/matrix_size[0]);
    for(int y = 0; y < matrix_size[1]; y++){
        ky = (-1.+y*2.0/matrix_size[1]);
        kspace_filter[x+y*matrix_size[0]] = std::complex<float>(.5+std::atan(50.*(.95-std::sqrt(kx*kx+ky*ky)))/M_PI,0.0);
    }
  }
  cu_kspace_filter = *(hoNDArray<float_complext>*)&kspace_filter;
  //write_nd_array(&kspace_filter, "kspace_filter.cplx");

}

<<<<<<< HEAD
hoNDArray<complext<float>> MFIOperator::MFI_apply(hoNDArray<complext<float>>& ho_image, hoNDArray<float> B0_map)
=======
hoNDArray<complext<float>> MFIOperator::MFI_Apply(hoNDArray<complext<float>> &ho_image,hoNDArray<float> B0_map)
>>>>>>> 78428447f8c051b458fbf61261aaddb589182734
{

  //Core MFI function - assumes MFI operator has already been prepared
  //Takes in blurred imaged (ho_image) and field map (B0_map) as inputs
  hoNDArray<complext<float>> output_image(&image_dims);
  hoNDArray<complext<float>> temp_image(&image_dims);
  output_image.fill(0.0);
  cuNDArray<complext<float>> cu_image(&image_dims);
  int mfc_index;

  //Upload image and transform
  cuNDArray<complext<float>> gridded_data((hoNDArray<float_complext>*)&ho_image);
  nfft_plan_->fft(gridded_data, NFFT_fft_mode::FORWARDS);

  //Rewind data to get first demodulated image
  for (int j = 0; j < (L-1)/2; j++){
      gridded_data *= *conj(&cu_phase_mask);
  }

  //Demod data, iFFT, and combine using MFI weights
  for(size_t j = 0; j<L; j++){
    if(j != 0){
      gridded_data *= cu_phase_mask;
    }
    cu_image = gridded_data;
    cu_image *= cu_kspace_filter;
    nfft_plan_->fft(cu_image, NFFT_fft_mode::BACKWARDS);
    temp_image = *(cu_image.to_host());
    for (size_t i = 0; i < temp_image.get_number_of_elements(); i++) {
      mfc_index = int(B0_map[i]+fmax)*L+j;
      output_image[i] += (MFI_C[mfc_index]*temp_image[i]);
    }
  }

  return output_image;

}

}
