/*
 * Copyright (c) 2007 Hypertriton, Inc. <http://hypertriton.com/>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Operations on vectors in R^3 using SSE3 operations.
 */

#include <config/have_opengl.h>
#include <config/have_sse3.h>

#if defined(HAVE_OPENGL) && defined(HAVE_SSE3)

#include <core/core.h>
#include "sg.h"

const SG_VectorOps3 sgVecOps3_SSE3 = {
	"sse3",
	SG_VectorZero3_SSE,			/* -31 clks */
	SG_VectorGet3_SSE,			/* -8 clks */
	SG_VectorSet3_FPU,			/* +8 clks */
	SG_VectorCopy3_FPU,			/* = */
	SG_VectorMirror3_SSE,			/* TODO */
	SG_VectorMirror3p_SSE,			/* TODO */
	SG_VectorLen3_FPU,			/* = */
	SG_VectorLen3p_FPU,			/* = */
	SG_VectorDot3_SSE3,			/* -26 clks */
	SG_VectorDot3p_SSE3,			/* -59 clks */
	SG_VectorDistance3_SSE,			/* -31 clks */
	SG_VectorDistance3p_SSE,		/* -27 clks */
	SG_VectorNorm3_SSE,			/* -87 clks */
	SG_VectorNorm3p_SSE,			/* -54 clks */
	SG_VectorNorm3v_FPU,			/* +147 clks */
	SG_VectorCross3_FPU,			/* TODO */
	SG_VectorCross3p_FPU,			/* TODO */
	SG_VectorNormCross3_FPU,		/* TODO */
	SG_VectorNormCross3p_FPU,		/* TODO */
	SG_VectorScale3_SSE,			/* -27 clks */
	SG_VectorScale3p_SSE,			/* -15 clks */
	SG_VectorScale3v_FPU,			/* = */
	SG_VectorAdd3_SSE,			/* -29 clks */
	SG_VectorAdd3p_SSE,			/* -15 clks */
	SG_VectorAdd3v_FPU,			/* = */
	SG_VectorAdd3n_SSE,			/* TODO */
	SG_VectorSub3_SSE,			/* -29 clks */
	SG_VectorSub3p_SSE,			/* -15 clks */
	SG_VectorSub3v_FPU,			/* = */
	SG_VectorSub3n_SSE,			/* TODO */
	SG_VectorAvg3_SSE,			/* TODO */
	SG_VectorAvg3p_SSE,			/* TODO */
	SG_VectorLERP3_SSE,			/* TODO */
	SG_VectorLERP3p_SSE,			/* TODO */
	SG_VectorElemPow3_SSE,			/* TODO */
	SG_VectorVecAngle3_SSE,			/* TODO */
	SG_VectorRotate3_SSE,			/* TODO */
	SG_VectorRotate3v_SSE			/* TODO */,
	SG_VectorRotateQuat3_SSE,		/* TODO */
	SG_VectorRotateI3_SSE,			/* TODO */
	SG_VectorRotateJ3_SSE,			/* TODO */
	SG_VectorRotateK3_SSE,			/* TODO */
};

#endif /* HAVE_OPENGL and HAVE_SSE3 */
