/*------------------------------------------------------------------------*/
/*                 Copyright 2010 Sandia Corporation.                     */
/*  Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive   */
/*  license for use of this work by or on behalf of the U.S. Government.  */
/*  Export of this program may require a license from the                 */
/*  United States Government.                                             */
/*------------------------------------------------------------------------*/

#include <gtest/gtest.h>
#include <mpi.h>

int gl_argc=0;
char** gl_argv=0;

int main(int argc, char **argv)
{
#ifdef STK_MESH_TRACE_ENABLED
    use_case::UseCaseEnvironment use_case_environment(&argc, &argv); 
#else
    MPI_Init(&argc, &argv);
#endif

    testing::InitGoogleTest(&argc, argv);

    gl_argc = argc;
    gl_argv = argv;

    int returnVal = RUN_ALL_TESTS();

#ifndef STK_MESH_TRACE_ENABLED
    MPI_Finalize();
#endif

    return returnVal;
}
