//
//  Shader.fsh
//  gl
//
//  Created by Guy Barrand on 9/3/10.
//  Copyright Apple Inc 2010. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
