// Copyright (C) 2019. Huawei Technologies Co., Ltd. All rights reserved.

// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE 
// WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


#include "sys.h"
#include "tensor_desc.h"
#include "type.h"
#include "error.h"
#include "tensor_computing_type.h"

#include "cpu/arm/fp16/concat_fp16.h"

EE concat_fp16(std::vector<TensorDesc> inputDesc, std::vector<void*> input, TensorDesc outputDesc, void* output, U32 concatDim)
{
    if (inputDesc.size() < 1) {
        CHECK_STATUS_WITH_RETURN(NOT_MATCH);
    }
    if(inputDesc.size() == 1) {
        memcpy(output, input[0], tensorNumBytes(outputDesc));
        return SUCCESS;
    }
    if (concatDim != 0 && concatDim != 1) {
        CHECK_STATUS_WITH_RETURN(NOT_SUPPORTED);
    }

    DataType odt, idt;
    DataFormat odf, idf;
    U32 on = 0, oc = 0, oh = 0, ow = 0,
        in = 0, ic = 0, ih = 0, iw = 0;
    U32 copySize;

    if(tensorIs4d(outputDesc)) {
        CHECK_STATUS_WITH_RETURN(tensor4dGet(outputDesc, &odt, &odf, &on, &oc, &oh, &ow));
        if (odt != DT_F16) {
            CHECK_STATUS_WITH_RETURN(NOT_MATCH);
        }

        char *out_ptr = (char *)output;
        //batch
        if(concatDim == 0) {
            for(U32 i = 0; i < inputDesc.size(); i++) {
                copySize = tensorNumElements(inputDesc[i]) * sizeof(F16);

                memcpy(out_ptr, input[i], copySize);
                out_ptr = out_ptr + copySize;
            }
            return SUCCESS;
        }
        //channel
        if(concatDim == 1) {
            for(U32 j = 0; j < on; j++) {
                for(U32 i = 0; i < inputDesc.size(); i++) {
                    CHECK_STATUS_WITH_RETURN(tensor4dGet(inputDesc[i], &idt, &idf, &in, &ic, &ih, &iw));
                    if (odf != idf) {
                        CHECK_STATUS_WITH_RETURN(NOT_MATCH);
                    }

                    copySize = tensorNumElements(inputDesc[i]) / in * sizeof(F16);

                    memcpy(out_ptr, (char *)input[i] + j * copySize, copySize);
                    out_ptr = out_ptr + copySize;
                }
            }
            return SUCCESS;
        }
    }
    else{
        return NOT_MATCH;
    }
    return NOT_SUPPORTED;
}