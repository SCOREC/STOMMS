Module eqd_module
    use EZspline_obj
    use EZspline
    implicit none
    ! eqdsk data
    integer, parameter :: eqd_neqdsk=3
    character (LEN=64) :: eqd_filename,eqd_outfilename
    integer :: eqd_mw, eqd_mh, eqd_mpsi ! eqd_mpsi is for the eqd file
    integer :: eqd_imfit
    character (LEN=10) :: eqd_case(6)
    real (kind=8) :: eqd_rdim, eqd_zdim, eqd_rzero, eqd_zmid  ! rdim : r size of grid box
    real (kind=8) :: eqd_rmaxis, eqd_zmaxis, eqd_ssimag, eqd_ssibry, eqd_bzero
    real (kind=8) :: eqd_simag, eqd_psibry, eqd_sifm, eqd_sifb   ! this line is not used
    real (kind=8) :: eqd_cpasma, eqd_xdum,eqd_sdum  
    real (kind=8) :: eqd_r_min, eqd_z_min   ! minimum value of the grid
    real (kind=8),allocatable :: eqd_fpol(:),eqd_pres(:),eqd_qpsi(:), eq_I(:) ! eq_I is for the eqd file
    real (kind=8),allocatable :: eqd_workk(:),eqd_ffprim(:),eqd_pprime(:)  !temp_variable, FF', P'
    real (kind=8),allocatable :: eqd_psirz(:,:), eqd_rgrid(:), eqd_zgrid(:)
    real (kind=8) :: eqd_drgrid,eqd_dzgrid,eqd_darea,eqd_dxsi, eqd_r_max, eqd_z_max
    real (kind=8),allocatable :: eqd_xsi(:), eqd_psi_grid(:)
    real (kind=8) :: eq_x_psi, eq_axis_psi   ! ES : 2015-06-06 

    !preprocessing parameters
    real (kind=8) :: eqd_axis_r, eqd_axis_z, eqd_axis_psi, eqd_xd_r, eqd_xd_z, eqd_x_psi

    ! separatrix and limiter 
    integer :: eqd_nlim, eqd_nbdry
    real (kind=8), allocatable ::  eqd_rbdry(:), eqd_zbdry(:), eqd_rlim(:), eqd_zlim(:)
  
    ! scale factor to psi
    real (kind=8) :: eqd_psi_factor

    ! eq_module defined for m3dc12g
    integer :: eq_mr, eq_mz, eq_mpsi

    ! data structure for equilbirum profile
    type eq_ftn_type
       integer :: shape
       real (8):: inx(3), iny(3)
       real (8):: sv(6)
       ! for arbitrary profile function - use pspline
       character (len=256) :: filename
       type (EZspline1_r8) :: spl
       real (8) :: min, max
    end type eq_ftn_type

    !type(eq_ftn_type) :: eq_tempi, eq_tempe, eq_den, eq_flowi, eq_flowe, eq_zeff
    type(eq_ftn_type) :: eq_tempi
    real (8) :: ptl_mass_i
    type(eq_ftn_type) :: eq_temps
    real (8) :: ptl_mass_s

    logical :: rev_psi  !!for reverse_psi

end module eqd_module


! interface to use the raw g file
module interpData
  use EZspline_obj
  use EZspline
  real(kind=8), dimension(4) :: boundBox
  type(EZspline2_r8) :: spl
  type(EZspline1_r8) :: spl_I
  real(kind=8) :: psi_max, psi_min
end module interpData

!=================================================== from XGC1 (S) : ES 2015-06-06
  subroutine eval_gri_val (r, z, val, ier)
    use eqd_module
    use interpData 
    use EZspline_obj
    use EZspline
    implicit none
    real(kind=8), intent(in) :: r, z
    real(kind=8), intent(out) :: val
    integer, intent(out) :: ier

    real(kind=8) :: vthi, tempi, B
    real(kind=8), parameter :: sml_e_charge=1.602176565D-19  !! electron charge (MKS)
    real(kind=8) :: Br, Bz, Bphi
    real(kind=8) :: psi, dpsi_dr, dpsi_dz, fi
    real(kind=8), external :: eq_ftn

    !=============================================Ti  (from function?) 
    tempi = eq_ftn(r,z,eq_tempi)
    !=============================================Ti  

    call eval_field_deriv(r, z, 1, 0, dpsi_dr, ier)
    call eval_field_deriv(r, z, 0, 1, dpsi_dz, ier)
    call eval_field_val(r,z,psi,ier)
    call eval_I_val (psi, fi, ier)
    vthi = sqrt(tempi*sml_e_charge/ptl_mass_i)
    Br= -dpsi_dz / r
    Bz= dpsi_dr / r
    Bphi= fi / r
    B = sqrt(Br*Br+Bz*Bz+Bphi*Bphi)
    val = vthi/(sml_e_charge*B/ptl_mass_i)
  end subroutine eval_gri_val

  subroutine eval_B_vec (r, z, vec, ier)
    use eqd_module
    use interpData 
    use EZspline_obj
    use EZspline
    implicit none
    real(kind=8), intent(in) :: r, z
    real(kind=8), dimension(3), intent(out) :: vec
    integer, intent(out) :: ier

    real(kind=8) :: Br, Bz, Bphi
    real(kind=8) :: psi, dpsi_dr, dpsi_dz, fi

    call eval_field_deriv(r, z, 1, 0, dpsi_dr, ier)
    call eval_field_deriv(r, z, 0, 1, dpsi_dz, ier)
    call eval_field_val(r,z,psi,ier)
    call eval_I_val (psi, fi, ier)
    vec(1)= -dpsi_dz / r  !! Br
    vec(2)= dpsi_dr / r   !! Bz
    vec(3)= fi / r      !! Bphi
  end subroutine eval_B_vec

  subroutine eval_B_val(r,z,val,ier)
    implicit none
    real(kind=8), intent(in) :: r, z
    real(kind=8), intent(out) :: val
    integer, intent(out) :: ier
    real(kind=8), dimension(3) :: Bvec

    call eval_B_vec(r,z,Bvec,ier)
    val = sqrt(Bvec(1)*Bvec(1)+Bvec(2)*Bvec(2)+Bvec(3)*Bvec(3))
  end subroutine eval_B_val
 
  subroutine eval_grs_val (r, z, val, ier)
    use eqd_module
    use interpData 
    use EZspline_obj
    use EZspline
    implicit none
    real(kind=8), intent(in) :: r, z
    real(kind=8), intent(out) :: val
    integer, intent(out) :: ier

    real(kind=8) :: vths, temps, B
    real(kind=8), parameter :: sml_e_charge=1.602176565D-19  !! electron charge (MKS)
    real(kind=8) :: Br, Bz, Bphi
    real(kind=8) :: psi, dpsi_dr, dpsi_dz, fi
    real(kind=8), external :: eq_ftn

    !=============================================Ti  (from function?) 
    temps = eq_ftn(r,z,eq_temps)
    !=============================================Ti  

    call eval_field_deriv(r, z, 1, 0, dpsi_dr, ier)
    call eval_field_deriv(r, z, 0, 1, dpsi_dz, ier)
    call eval_field_val(r,z,psi,ier)
    call eval_I_val (psi, fi, ier)
    vths = sqrt(temps*sml_e_charge/ptl_mass_s)
    Br= -dpsi_dz / r
    Bz= dpsi_dr / r
    Bphi= fi / r
    B = sqrt(Br*Br+Bz*Bz+Bphi*Bphi)
    val = vths/(sml_e_charge*B/ptl_mass_s)
  end subroutine eval_grs_val
 
  ! pspline funtion value
  subroutine ftn_evaluation(spl,psi,val)
    use EZspline_obj
    use EZspline
    implicit none
    type (EZspline1_r8) :: spl
    real (8) :: psi, val
    integer :: ierr

    call ezspline_interp(spl,psi,val,ierr)

  end subroutine ftn_evaluation

  subroutine eval_ti_val (psi, val, ier)
    use eqd_module 
    use EZspline_obj
    use EZspline
    implicit none
    real(kind=8), intent(in) :: psi
    real(kind=8), intent(out) :: val
    integer, intent(out) :: ier

    real(kind=8) :: psi2
  
    psi2 = psi
    if(psi > eq_tempi%max) psi2 = eq_tempi%max
    if(psi < eq_tempi%min) psi2 = eq_tempi%min
    call EZspline_interp(eq_tempi%spl,psi2,val,ier)
    call EZspline_error(ier)
  end subroutine eval_ti_val

  subroutine initial_setting_prf (xpsi, axispsi, ptl_mass_au)
    use eqd_module
    implicit none
  
    real (kind=8), intent(in) :: xpsi, axispsi, ptl_mass_au 
    eq_x_psi=xpsi
    eq_axis_psi=axispsi
    ptl_mass_i= ptl_mass_au * 1.660539040D-27
  end subroutine initial_setting_prf

  ! function evaluation
  real(8) function eq_ftn(r,z,ftn)
    use eqd_module
    use interpData
    implicit none
    type(eq_ftn_type) :: ftn
    real(8) :: r, z, tmp, tmp2, tmp3, tmp4
    integer :: ier

    call eval_field_val(r,z,tmp,ier)
    if(rev_psi) tmp = -tmp
    tmp = tmp-eq_axis_psi

    select case(ftn%shape)
    case(0) ! constant
       eq_ftn=ftn%iny(1)
    case(1) ! hyperbolic tanh
       eq_ftn = ftn%sv(2)*tanh((ftn%inx(1)-tmp)*ftn%sv(3))+ftn%sv(1)
    case(2) ! linear
       eq_ftn = ftn%sv(1)*tmp + ftn%sv(2)
    case(3) ! a exp(-B w tanh( (r-r0)/w ))
       eq_ftn = ftn%iny(1)*exp(ftn%sv(1)*tanh((ftn%inx(1)-tmp)*ftn%sv(2)))
    case(4)
       eq_ftn = ftn%sv(2)*dtanh((ftn%inx(1)-tmp)*ftn%sv(3))+ftn%sv(1)
       if(tmp < ftn%inx(1)-0.5D0*ftn%inx(2) ) eq_ftn = eq_ftn + ftn%sv(4)*sqrt(tmp) + ftn%sv(5)
    case(5)
       tmp2=ftn%sv(3)*( ftn%inx(1)-sqrt(ftn%inx(1)*tmp) )  ! z
       tmp3=exp(tmp2)                           ! expz
       tmp4=exp(-tmp2)                          ! expmz
       ! A * ( (1+z*slope)*expz - expmz )/(expz + expmz) + B
       eq_ftn = ftn%sv(2)*( (1+tmp2*ftn%sv(4))*tmp3 - tmp4 )/(tmp3+tmp4) + ftn%sv(1)
    case(-1)
       tmp=min(max(tmp+eq_axis_psi,ftn%min),ftn%max)
       call ftn_evaluation(ftn%spl,tmp,eq_ftn)
    case default
       print *, 'Invalid shape number in eq_ftn', ftn%shape
       stop
    end select

  end function eq_ftn


  subroutine eq_ftn_setup_ti(eq_tempi_shape, eq_tempi_file, strlen, &
                             eq_tempi_x1, eq_tempi_x2, eq_tempi_x3, &
                             eq_tempi_v1, eq_tempi_v2, eq_tempi_v3)
     use eqd_module
     implicit none
     integer :: eq_tempi_shape
     integer :: strlen
     character (len=strlen) :: eq_tempi_file
     real (kind=8) :: eq_tempi_x1, eq_tempi_x2, eq_tempi_x3
     real (kind=8) :: eq_tempi_v1, eq_tempi_v2, eq_tempi_v3

     eq_tempi%shape = eq_tempi_shape
     eq_tempi%filename = eq_tempi_file
     eq_tempi%inx(1) = eq_tempi_x1* (eq_x_psi-eq_axis_psi)     
     eq_tempi%inx(2) = eq_tempi_x2* (eq_x_psi-eq_axis_psi) 
     eq_tempi%inx(3) = eq_tempi_x3* (eq_x_psi-eq_axis_psi) 
     eq_tempi%iny(1) = eq_tempi_v1
     eq_tempi%iny(2) = eq_tempi_v2
     eq_tempi%iny(3) = eq_tempi_v3

#ifdef DEBUG
     write(*,1310)
     write(*,1311) eq_tempi_file 
     1310 format(T9,'* Ion temperature data filename * ' /)
     1311 format(T17,A /)
#endif

     call eq_ftn_setup(eq_tempi)

  end subroutine eq_ftn_setup_ti

  subroutine eq_ftn_setup_ts(ptl_mass_s_au, eq_temps_shape, eq_temps_file, strlen, &
                             eq_temps_x1, eq_temps_x2, eq_temps_x3, &
                             eq_temps_v1, eq_temps_v2, eq_temps_v3)
     use eqd_module
     implicit none
     real (kind=8) :: ptl_mass_s_au
     integer :: eq_temps_shape
     integer :: strlen
     character (len=strlen) :: eq_temps_file
     real (kind=8) :: eq_temps_x1, eq_temps_x2, eq_temps_x3
     real (kind=8) :: eq_temps_v1, eq_temps_v2, eq_temps_v3

     ptl_mass_s = ptl_mass_s_au * 1.660539040D-27

     eq_temps%shape = eq_temps_shape
     eq_temps%filename = eq_temps_file
     eq_temps%inx(1) = eq_temps_x1* (eq_x_psi-eq_axis_psi)     
     eq_temps%inx(2) = eq_temps_x2* (eq_x_psi-eq_axis_psi) 
     eq_temps%inx(3) = eq_temps_x3* (eq_x_psi-eq_axis_psi) 
     eq_temps%iny(1) = eq_temps_v1
     eq_temps%iny(2) = eq_temps_v2
     eq_temps%iny(3) = eq_temps_v3

#ifdef DEBUG
     write(*,1310)
     write(*,1311) eq_temps_file 
     1310 format(T9,'* Subgrid temperature data filename * ' /)
     1311 format(T17,A /)
#endif

     call eq_ftn_setup(eq_temps)

  end subroutine eq_ftn_setup_ts


  !setup eq_ftn functions
  subroutine eq_ftn_setup(ftn)
    use eqd_module
    implicit none
    type(eq_ftn_type) :: ftn

    select case(ftn%shape)
    case(0) ! constant
       continue  ! do nothing
    case(1) ! hyperbolic tanh
       ftn%sv(1)=0.5D0*(ftn%iny(1)+ftn%iny(2))
       ftn%sv(2)=0.5D0*(ftn%iny(1)-ftn%iny(2))
       ftn%sv(3)=2D0/ftn%inx(2)
    case(2) ! linear
       ftn%sv(1)=(ftn%iny(2)-ftn%iny(1))/ (ftn%inx(2)-ftn%inx(1)) ! slope
       ftn%sv(2)=ftn%iny(1)-ftn%sv(1)*ftn%inx(1)     ! offset
    case(3) ! A exp(B*w tanh( (r-r0)/w )
       ftn%sv(1)=ftn%inx(2)/ftn%inx(3)
       ftn%sv(2)=1D0/ftn%inx(2)
    case(4)  ! modified hyperbolic tanh : sqrt linear + tanh --> with if statement
       ftn%sv(1)=0.5D0*(ftn%iny(1)+ftn%iny(2))
       ftn%sv(2)=0.5D0*(ftn%iny(1)-ftn%iny(2))
       ftn%sv(3)=2D0/ftn%inx(2)
       ftn%sv(4)=(ftn%iny(3)-ftn%iny(1)) / (sqrt(ftn%inx(3)) - sqrt(ftn%inx(1)-0.5D0*ftn%inx(2)))
       ftn%sv(5)= - ftn%sv(4)*sqrt(ftn%inx(1))
    case(5)   ! modified hyperbolic tanh : sqrt linear + tanh --> smooth function
       ! A ( (1+z slope) e^z - e^-z )/(e^z + e^-z) + B
       ! z= 4 ( center - sqrt(center psi) )/ width
       ! iny(1) - edge_val : ped_top
       ! iny(2) - out_val : ped bottom
       ! inx(1) - center ,  inx(2) - width
       ! iny(3) - core(axis)_val , inx(3) - core(axis)_psi
       ftn%sv(1)=0.5D0*(ftn%iny(1)+ftn%iny(2))    !
       ftn%sv(2)=0.5D0*(ftn%iny(1)-ftn%iny(2))    ! height
       ftn%sv(3)=4D0/ftn%inx(2)                   ! width inverse
       ftn%sv(4)=(ftn%iny(3)-ftn%iny(1)) / (sqrt(ftn%inx(3)) - sqrt(ftn%inx(1))) &
            /( - (ftn%sv(2)+1D-99) * ftn%sv(3) * sqrt(ftn%inx(1)) )  ! slope
    case(-1) ! arbitrary profile - file input
       call init_ftn_spline(ftn)
    case default
       print *, 'Invalid shape number in eq_ftn_setup', ftn%shape
       stop
    end select
  end subroutine eq_ftn_setup

  subroutine init_ftn_spline(ftn)
    use eqd_module
    implicit none
    type(eq_ftn_type) :: ftn
    integer, parameter :: funit=16
    real (8), allocatable :: psi(:), var(:)
    integer :: num, i, flag
    integer :: ierr, BCS1(2)

    ! open file
    open(unit=funit,file=ftn%filename,action='read')
    read(funit,*) num
    if(num<=0) then
       print *, 'error in profile ftn init : invalid number of data', num, ftn%filename
       stop
    endif

    !mem allocation
    allocate(psi(num),var(num))

    ! read data
    do i=1, num
       read(funit,*) psi(i),var(i)
    enddo

    read(funit,*) flag
    if(flag/=-1) then
       print *, 'error in profile ftn init : invalid number of data or ending flag -1 is not set correctly', flag, ftn%filename
       stop
    endif
    close(16)

    open(unit=funit,file=trim(ftn%filename)//'.used',status='replace')
    do i=1, num
       write(funit,*) psi(i),var(i)
    enddo

    ! NOTE THAT DUE TO THIS CONVERTING, this routine should be used after finding axis psi
    ! convert normalization
    psi=psi*(eq_x_psi-eq_axis_psi) + eq_axis_psi

    ! save psi range
    ftn%min=psi(1)
    ftn%max=psi(num)

    ! set size and boundary condition
    BCS1=0
    call ezspline_init(ftn%spl,num,BCS1,ierr)
    call EZspline_error(ierr)

    ! get coefficient
    ftn%spl%x1=psi
    call ezspline_setup(ftn%spl,var,ierr)
    call EZspline_error(ierr)

    deallocate(psi,var)
  end subroutine init_ftn_spline

!=================================================== from XGC1 (E) : ES 2015-06-06


!eqd_g_tag = 0 : efit data format, eqd_g_tag=1 : eqd format
!*******************************************************
subroutine init_ez_spline (reverse_psi, eqd_g_tag)
!*******************************************************
    use EZspline_obj
    use EZspline
    use interpData
    use eqd_module
    implicit none
    integer, intent(in) :: reverse_psi
    integer, intent(in) :: eqd_g_tag
    real (kind=8) ,external:: interpol_raw
    integer :: i,j, max_mpsi, max_mr
    integer, dimension(2) :: BCS1, BCS2
    integer :: ier, r_der, z_der, ir, iz
    if(reverse_psi .eq. 0) then
       rev_psi = .false.
    else 
       rev_psi = .true.
    endif
    boundBox(1)=eqd_rgrid(1)
    boundBox(2)=eqd_zgrid(1) 
    boundBox(3)=eqd_rgrid(eqd_mw)
    boundBox(4)=eqd_zgrid(eqd_mh)
    BCS1 = 0 ! not a knot
    BCS2 = 0 ! not a knot
    call EZspline_init(spl,eqd_mw,eqd_mh,BCS1,BCS2,ier)
    call EZspline_error(ier)
    spl%x1 = eqd_rgrid
    spl%x2 = eqd_zgrid
#ifdef DEBUG
       print *, 'EZspline init for spl is completed'
#endif       

    call EZspline_setup(spl,eqd_psirz,ier)
    call EZspline_error(ier)
#ifdef DEBUG
       print *, 'EZspline setup for psirz is completed'
#endif       

    if(eqd_g_tag .eq. 0) then
       !efit data
       BCS1 = 0
       call EZspline_init(spl_I, eqd_mw, BCS1, ier)
       call EZspline_error(ier)
#ifdef DEBUG
       print *, 'EZspline init for spl_I is completed'
       print *, 'If you see an error around here, it is likely that &
                 the data has maximum psi at magnetic axis. use reverse_psi in input' 
#endif       
       if(reverse_psi > 0) then
          eqd_psi_grid(:) = eqd_psi_grid(:) * (-1)
       end if

       spl_I%x1 = eqd_psi_grid
       call EZspline_setup(spl_I, dabs(eqd_fpol), ier)
       call EZspline_error(ier)

#ifdef DEBUG
       print *, 'EZspline setup for eqd_fpol is completed'
#endif       
       psi_max = eqd_psi_grid(eqd_mw)
       psi_min = eqd_psi_grid(1)
    else
       !eqd data
       BCS1 = 0
       call EZspline_init(spl_I, eqd_mpsi, BCS1, ier)
       call EZspline_error(ier)
       
       if(reverse_psi > 0) then
          eqd_psi_grid(:) = eqd_psi_grid(:) * (-1)
       end if

       spl_I%x1 = eqd_psi_grid
       call EZspline_setup(spl_I, dabs(eq_I), ier)
       call EZspline_error(ier)

       psi_max = eqd_psi_grid(eqd_mpsi)
       psi_min = eqd_psi_grid(1)
   endif

#ifdef DEBUG
    print *, 'init_ez_spline: psi max=', psi_max
#endif
end subroutine init_ez_spline

!*******************************************************
subroutine eval_field_val (r, z, val, ier)
!*******************************************************
  use interpData
  use EZspline_obj
  use EZspline
  implicit none
  real(kind=8), intent(in) :: r, z
  real(kind=8), intent(out) :: val
  integer, intent(out) :: ier
  if(r<boundBox(1) .or. r>boundBox(3).or. z<boundBox(2) .or. z>boundBox(4)) then
     ier=1;
     return;
  end if
  call EZspline_interp(spl,r,z,val,ier)
  call EZspline_error(ier)
end subroutine eval_field_val

subroutine eval_field_grad(r, z, val, ier)
  use interpData
  use EZspline_obj
  use EZspline
  implicit none
  real(kind=8), intent(in) :: r, z
  real(kind=8), intent(out), dimension(2) :: val
  integer, intent(out) :: ier
  if(r<boundBox(1) .or. r>boundBox(3).or. z<boundBox(2) .or. z>boundBox(4)) then
     ier=1;
     return;
  end if
  call EZspline_gradient(spl, r, z, val, ier)
  call EZspline_error(ier)
end subroutine eval_field_grad

!*******************************************************
subroutine eval_field_deriv(r, z, dr, dz, val, ier)
!*******************************************************
  use interpData
  use EZspline_obj
  use EZspline
  implicit none
  real(kind=8), intent(in) :: r, z
  integer, intent(in) :: dr, dz
  real(kind=8), intent(out) :: val
  integer, intent(out) :: ier
  if(r<boundBox(1) .or. r>boundBox(3).or. z<boundBox(2) .or. z>boundBox(4)) then
     ier=1;
     return;
  end if

  call EZspline_derivative(spl, dr, dz, r, z, val, ier)
  call EZspline_error(ier)
end subroutine eval_field_deriv

!*******************************************************
subroutine eval_I_val (psi, val, ier)
!*******************************************************
  use interpData
  use EZspline_obj
  use EZspline
  implicit none
  real(kind=8) :: psi2
  real(kind=8), intent(in) :: psi
  real(kind=8), intent(out) :: val
  integer, intent(out) :: ier

  psi2 = psi
  if(psi > psi_max) psi2 = psi_max
  if(psi < psi_min) psi2 = psi_min
  call EZspline_interp(spl_I,psi2,val,ier)
  call EZspline_error(ier)
end subroutine eval_I_val

!*******************************************************
subroutine eval_I_grad(psi, val, ier)
!*******************************************************
  use interpData
  use EZspline_obj
  use EZspline
  implicit none
  real(kind=8) :: psi2
  real(kind=8), intent(in) :: psi
  real(kind=8), intent(out) :: val
  integer, intent(out) :: ier

  psi2 = psi
  if(psi > psi_max) psi2 = psi_max
  if(psi < psi_min) psi2 = psi_min
  call EZspline_derivative(spl_I, 1, psi2, val, ier)
  call EZspline_error(ier)
end subroutine eval_I_grad

!*******************************************************
subroutine get_b_box( bbox)
!*******************************************************
  use interpData
  implicit none
  real(kind=8), intent(out), dimension(4) :: bbox
  bbox=boundBox;
end subroutine get_b_box  

!*******************************************************
subroutine get_psi_grid_num( mw, mh )
!*******************************************************
    use eqd_module
    implicit none
    integer, intent(out) :: mw, mh
    mw = eqd_mw
    mh = eqd_mh
end subroutine get_psi_grid_num

!*******************************************************
subroutine get_psi_and_its_grid( rgrid, zgrid, psirz )
!*******************************************************
    use eqd_module
    implicit none
    real(kind=8), intent(out), dimension(eqd_mw) :: rgrid
    real(kind=8), intent(out), dimension(eqd_mh) :: zgrid
    real(kind=8), intent(out), dimension(eqd_mw*eqd_mh) :: psirz

    integer ::i, j
    
    rgrid = eqd_rgrid
    zgrid = eqd_zgrid
!$omp parallel private(i,j)
!$omp do collapse(2) 
    do i=1, eqd_mh
        do j=1, eqd_mw
            psirz(j+eqd_mh*(i-1)) = eqd_psirz(j,i)
        enddo
    enddo
!$omp end parallel
end subroutine get_psi_and_its_grid

!*******************************************************
subroutine set_eqd_psi_factor (fac)
!*******************************************************
  use eqd_module
  implicit none

  real (kind=8), intent(in) :: fac
  eqd_psi_factor=fac;
end subroutine

! read gfile code
! relevant source : read_eqdsk in gengrid2 
!*******************************************************
subroutine readgfile(gfilename, strlens)
!*******************************************************
  use eqd_module
  implicit none

  integer, intent(in) :: strlens
  character(len=strlens), intent(in) :: gfilename
  real (kind=8), parameter :: pi=3.1415926535897932, tmu=2.0e-07
  real (kind=8), parameter :: twopi=pi*2.
  integer :: i,j
  real (kind=8), allocatable :: qpsi(:), tmp_psirz(:,:)

  write(*,1300)
  write(*,1301) gfilename
  1300 format(T9,'* EFIT data filename * ' /)
  1301 format(T17,A /)

  open(unit=eqd_neqdsk,file=gfilename,status='old')
  read (eqd_neqdsk,2000) (eqd_case(i),i=1,6),eqd_imfit,eqd_mw,eqd_mh
#ifdef DEBUG
  write (6, 4000) (EQD_case(i), i=1,6)
4000 format(' Case=', 6a10)
  write (6,*) ' imfit=', EQD_imfit, ' mw=', EQD_mw, ' mh=', EQD_mh
#endif

  !memory allocation
  allocate(eqd_fpol(eqd_mw), eqd_pres(eqd_mw), eqd_qpsi(eqd_mw), eqd_workk(eqd_mw), &
       eqd_ffprim(eqd_mw), eqd_pprime(eqd_mw),eqd_psirz(eqd_mw,eqd_mh), eqd_xsi(eqd_mw))
  allocate(eqd_rgrid(eqd_mw),eqd_zgrid(eqd_mh))
  allocate(eqd_psi_grid(eqd_mw))
  read (eqd_neqdsk,2020) eqd_rdim,eqd_zdim,eqd_rzero,eqd_r_min,eqd_zmid
#ifdef DEBUG
  print *, 'eqd_zmid = ', eqd_zmid
#endif
  read (eqd_neqdsk,2020) eqd_rmaxis,eqd_zmaxis,eqd_ssimag,eqd_ssibry,eqd_bzero
  read (eqd_neqdsk,2020) eqd_cpasma,eqd_xdum,eqd_xdum,eqd_rmaxis,eqd_xdum
  read (eqd_neqdsk,2020) eqd_zmaxis,eqd_xdum,eqd_sdum,eqd_xdum,eqd_xdum
#ifdef DEBUG
  print *, 'eqd_xdum = ', eqd_xdum
#endif
  read (eqd_neqdsk,2020) (eqd_fpol(i),i=1,eqd_mw)
#ifdef DEBUG
  print *, 'eqd_fpol last data = ', eqd_fpol(eqd_mw)
#endif
  read (eqd_neqdsk,2020) (eqd_pres(i),i=1,eqd_mw)
#ifdef DEBUG
  print *, 'eqd_pres last data = ', eqd_pres(eqd_mw)
#endif
  read (eqd_neqdsk,2020) (eqd_workk(i),i=1,eqd_mw)
#ifdef DEBUG
  print *, 'eqd_workk last data = ', eqd_workk(eqd_mw)
#endif
  eqd_drgrid=eqd_rdim/dfloat(eqd_mw-1)
  eqd_dzgrid=eqd_zdim/dfloat(eqd_mh-1)
  eqd_z_min=eqd_zmid-eqd_zdim/2.
  
  eqd_darea=eqd_drgrid*eqd_dzgrid
  do i=1,eqd_mw
     if (eqd_imfit.ge.0) eqd_ffprim(i)=-eqd_workk(i)/(twopi*tmu)
     if (eqd_imfit.lt.0) eqd_ffprim(i)=-eqd_workk(i)
  enddo
  read (eqd_neqdsk,2020) (eqd_workk(i),i=1,eqd_mw)
  do i=1,eqd_mw
     eqd_pprime(i)=-eqd_workk(i)
  enddo

  read (eqd_neqdsk,2020) ((eqd_psirz(i,j),i=1,eqd_mw),j=1,eqd_mh)
#ifdef DEBUG
  print *, 'eqd_psirz last data = ', eqd_psirz(eqd_mw,eqd_mh)
#endif

  allocate(qpsi(eqd_mw))
  read(eqd_neqdsk,5001) (qpsi(i),i=1,eqd_mw)
#ifdef DEBUG
  open(unit=999,file='eqd_qpsi.dat')
  write (999,*) qpsi
  close(999)
  print *, 'qpsi first and last data = ', qpsi(1), qpsi(2), qpsi(10),  qpsi(eqd_mw)
#endif
  read (eqd_neqdsk,5000) eqd_nbdry,eqd_nlim
#ifdef DEBUG
  print *,'readgfile: eqd_nbdry=',eqd_nbdry,', eqd_nlim=', eqd_nlim
#endif
  5000 format(2i5)
  allocate( eqd_rbdry(eqd_nbdry),eqd_zbdry(eqd_nbdry),eqd_rlim(eqd_nlim),eqd_zlim(eqd_nlim))
  read(eqd_neqdsk,5001) (eqd_rbdry(i),eqd_zbdry(i),i=1,eqd_nbdry)
  read(eqd_neqdsk,5001) (eqd_rlim(i),eqd_zlim(i),i=1,eqd_nlim)
  5001 format(5e16.9) 
  5003 format(e19.13,' ',e19.13)
  close(eqd_neqdsk)


  ! change from SH email
  eqd_psirz=eqd_psi_factor*eqd_psirz
  eqd_ssimag=eqd_psi_factor*eqd_ssimag
  eqd_ssibry=eqd_psi_factor*eqd_ssibry

  eqd_r_max=eqd_r_min+eqd_rdim
  eqd_z_max=eqd_z_min+eqd_zdim

  do i=1, eqd_mw
     eqd_rgrid(i)=eqd_r_min+(eqd_r_max-eqd_r_min)/real(eqd_mw-1) * real(i-1)!-eqd_rmaxis
  enddo
  do i=1, eqd_mh
     eqd_zgrid(i)=eqd_z_min+(eqd_z_max-eqd_z_min)/real(eqd_mh-1) * real(i-1)!-eqd_zmaxis
  enddo

  do i=1, eqd_mw
    !This is from axis to separatrix, i.e., 0 --> 1
    eqd_psi_grid(i)=real(i-1)/real(eqd_mw-1)*(eqd_ssibry-eqd_ssimag)+eqd_ssimag
  enddo

#ifdef DEBUG
  open(unit=999,file='eqd_psi_grid.dat')
  do i=1, eqd_mw
      write(999,*) eqd_psi_grid(i)
  enddo
  close(999)

  open(unit=999,file='eqd_psirz.dat')
  do i=1, eqd_mh
     do j=1, eqd_mw     
        write (999,*) eqd_rgrid(j), eqd_zgrid(i), eqd_psirz(j,i)
     enddo
  enddo
  close(999)
#endif
#ifdef DEBUG
  print *, "axis (r,z) = psi : ", eqd_rmaxis, eqd_zmaxis, eqd_bzero
#endif
  
  1020 format (1hx,i5,1h.,i3)
  1040 format (8h  EFIT  )
  1050 format (3h   ,a5)
  1060 format (a5,3h   )
  1070 format (3h # ,i5)
  1080 format (2h ,i4,2hms)
  2000 format (6a8,3i4)
  2020 format (5e16.9)
  2022 format (2i5)
  2024 format (i5,e16.9,i5)
  242 format (i6)
  244 format (i5)
  255 format (i4)
  3033 format (i6)
end subroutine readgfile

! read eqd file
! required variables by meshing : eqd_rgrid, eqd_zgrid, eqd_psirz, eqd_mw, eqd_mh, eqd_fpol 
!*******************************************************
subroutine readeqdfile(eqdfilename, strlens)
!*******************************************************
  use eqd_module
  implicit none
  integer, intent(in) :: strlens
  character(len=strlens), intent(in) :: eqdfilename
  character(len=80) :: eq_header
  integer :: i,j, end_flag
  real (kind=8), allocatable :: qpsi(:), tmp_psirz(:,:)
  real (kind=8) :: eq_axis_b, eq_x_r, eq_x_z, eq_x_psi_loc !Actually dummy

  write(*,1300)
  write(*,1301) eqdfilename
  1300 format(T9,'* eqdsk filename * ' /)
  1301 format(T17,A /)

  open(9,file=eqdfilename, status='old')
  read(9,300) eq_header
  read(9,200) eqd_mw, eqd_mh, eqd_mpsi

  allocate(eqd_psi_grid(eqd_mpsi), eqd_rgrid(eqd_mw),eqd_zgrid(eqd_mh))
  allocate(eq_I(eqd_mpsi), eqd_psirz(eqd_mw,eqd_mh))

  read(9,100) eqd_r_min, eqd_r_max, eqd_z_min, eqd_z_max
  read(9,100) eqd_rmaxis, eqd_zmaxis,eq_axis_b ! axis r, axis_z, axis_b
  read(9,100) eq_x_psi_loc, eq_x_r, eq_x_z
  read(9,100) (eqd_psi_grid(i), i=1, eqd_mpsi)

#ifdef DEBUG
  print *, "axis (r,z) = psi : ", eqd_rmaxis, eqd_zmaxis, eq_axis_b
#endif
  
  read(9,100) (eq_I(i), i=1,eqd_mpsi)
  read(9,100) ((eqd_psirz(i,j) , i=1, eqd_mw),j=1, eqd_mh)
  read(9,200) end_flag
  print *, eqd_mw, eqd_mh
  open(unit=999,file='eqd_psirz')
  write (999,*) eqd_psirz
  close(999) 

  if(end_flag/=-1) then
    !additional eq. input
    print *, 'wrong file format'
    stop
  endif

  do i=1, eqd_mw
     eqd_rgrid(i)=eqd_r_min+(eqd_r_max-eqd_r_min)/real(eqd_mw-1) * real(i-1)
  enddo
  do i=1, eqd_mh
     eqd_zgrid(i)=eqd_z_min+(eqd_z_max-eqd_z_min)/real(eqd_mh-1) * real(i-1)!-eqd_zmaxis
  enddo
  eqd_nlim = 4
  allocate(eqd_rlim(eqd_nlim))
  allocate(eqd_zlim(eqd_nlim))

  eqd_rlim(1) = eqd_rgrid(1)
  eqd_zlim(1) = eqd_zgrid(1)

  eqd_rlim(2) = eqd_rgrid(1)
  eqd_zlim(2) = eqd_zgrid(eqd_mh)

  eqd_rlim(3) = eqd_rgrid(eqd_mw)
  eqd_zlim(3) = eqd_zgrid(eqd_mh)

  eqd_rlim(4) = eqd_rgrid(eqd_mw)
  eqd_zlim(4) = eqd_zgrid(1)

100  format(4(e19.13,1x))
200  format(8I8)
201  format(3I8)
300  format(a80)

end subroutine readeqdfile

!*******************************************************
subroutine get_sep_pts (x,y, numPts)
!*******************************************************
  use eqd_module
  implicit none
  real(kind=8), dimension(eqd_nbdry), intent(out) :: x,y 
  integer, intent(out) :: numPts
  integer :: i
  numPts=eqd_nbdry
  x(1:numPts)=eqd_rbdry(1:numPts);
  y(1:numPts)=eqd_zbdry(1:numPts);
end subroutine get_sep_pts

!*******************************************************
subroutine get_bd_pts (x,y, numPts)
!*******************************************************
  use eqd_module
  implicit none
  real(kind=8), dimension(eqd_nlim), intent(out) :: x,y
  integer, intent(out) :: numPts
  integer :: i
  numPts=eqd_nlim
  x(1:numPts)=eqd_rlim(1:numPts);
  y(1:numPts)=eqd_zlim(1:numPts);
end subroutine get_bd_pts

!*******************************************************
subroutine get_num_bd_pts(n)
!*******************************************************
  use eqd_module
  implicit none
  integer, intent(out) :: n
  n = eqd_nlim
end subroutine get_num_bd_pts

! at x pt div is zero
!*******************************************************
subroutine get_x_pt (xPt)
!*******************************************************
  use eqd_module
  implicit none
  real(kind=8), dimension(2), intent(out) :: xPt
  real(kind=8), dimension(2):: divbuff
  real(kind=8) :: minDiv
  integer :: i, ier
  minDiv=1.e10
  do i=1,eqd_nbdry
    call eval_field_grad(eqd_rbdry(i),eqd_zbdry(i),divbuff,ier)
    if(abs(divbuff(1))+abs(divbuff(2))<minDiv) then
      minDiv=abs(divbuff(1))+abs(divbuff(2))
      xPt(1)=eqd_rbdry(i)
      xPt(2)=eqd_zbdry(i)
    end if
  end do
  print *, 'get_x_pt: xpt=', xPt
end subroutine get_x_pt

!*******************************************************
subroutine get_axis(axisPt)
!*******************************************************
  use eqd_module
  implicit none
  real(kind=8), dimension(2), intent(out) :: axisPt
  axisPt(1)=eqd_rmaxis
  axisPt(2)=eqd_zmaxis
end subroutine get_axis 

!*******************************************************
subroutine remove_duplicate_pt (inputx, inputy, numpt)
!*******************************************************
  implicit none
  real(kind=8), dimension(numpt), intent(inout) ::  inputx, inputy
  real(kind=8), dimension(numpt) :: xbuff, ybuff
  integer, intent(inout) :: numpt
  integer :: i,ptnext, ptidx
  ptidx=1;
  do i=1,numpt
     ptnext=i+1
     if(i==numpt) ptnext=1
     if(abs(inputx(i)-inputx(ptnext))+abs(inputy(i)-inputy(ptnext)) .ge. 1e-10) then
       xbuff(ptidx)=inputx(i)
       ybuff(ptidx)=inputy(i)
       ptidx=ptidx+1
     end if
  end do
  print *, "remove_duplicate_pt: remove dup pts=",numpt-ptidx+1
  numpt=ptidx-1
  inputx(1:numpt)=xbuff(1:numpt)
  inputy(1:numpt)=ybuff(1:numpt)
end subroutine remove_duplicate_pt
