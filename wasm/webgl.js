
  /////////////////////////////////////////////////
  /// tools : /////////////////////////////////////
  /////////////////////////////////////////////////
  //function get_element_content(a_element) {
  //  var str = '';
  //  var k = a_element.firstChild;
  //  while (k) {
  //    if (k.nodeType == 3) str += k.textContent;
  //    k = k.nextSibling;
  //  }
  //  return str;
  //}

  /////////////////////////////////////////////////
  /// code using global variables : ///////////////
  /////////////////////////////////////////////////
  var g_gl;

  var g_shader_program;

  var g_sampler;
  var g_tex_on;
  var g_point_size;

  var g_one_pos;
  var g_one_normal;
  var g_one_color;
  var g_one_tex;
  var g_model_matrix;
  var g_proj_matrix;
  var g_normal_matrix;
  var g_color;
  var g_pos_color;
  var g_normal;
  var g_pos_normal;
  var g_light_on;
  var g_light_direction;
  var g_light_color;

  function create_shader_program() {
    //var str = get_element_content(document.getElementById('fragment_shader'));
    var str = "\
#ifdef GL_ES\n\
  precision highp float;\n\
#endif\n\
  uniform sampler2D sampler;\n\
  varying vec4 frag_color;\n\
  varying float frag_tex_on;\n\
\n\
  varying vec2 frag_tex_coords;\n\
  void main(void) {\n\
    if(frag_tex_on==1.0) {\n\
      gl_FragColor = frag_color*texture2D(sampler,vec2(frag_tex_coords.s,frag_tex_coords.t));\n\
    } else {\n\
      gl_FragColor = frag_color;\n\
    }\n\
  }\n\
";
    var fragment_shader = g_gl.createShader(g_gl.FRAGMENT_SHADER);
    g_gl.shaderSource(fragment_shader,str);
    g_gl.compileShader(fragment_shader);
    if (!g_gl.getShaderParameter(fragment_shader,g_gl.COMPILE_STATUS)) {
      alert(g_gl.getShaderInfoLog(fragment_shader));
    }

    //var str = get_element_content(document.getElementById('vertex_shader'));
    var str = "\
  attribute vec3 one_pos;\n\
  attribute vec3 one_normal;\n\
  attribute vec4 one_color;\n\
  attribute vec2 one_tex;\n\
  uniform mat4 model_matrix;\n\
  uniform mat4 proj_matrix;\n\
  uniform mat4 normal_matrix;\n\
  uniform vec4 color;\n\
  uniform vec4 pos_color;\n\
  uniform vec3 normal;\n\
  uniform vec3 pos_normal;\n\
  uniform bool tex_on;\n\
  uniform bool light_on;\n\
  uniform vec3 light_direction;\n\
  uniform vec4 light_color;\n\
  uniform float point_size;\n\
\n\
  varying float frag_tex_on;\n\
  varying vec4 frag_color;\n\
  varying vec2 frag_tex_coords;\n\
\n\
  void main(void) {\n\
    frag_tex_on = 0.0;\n\
    if(tex_on) frag_tex_on = 1.0;\n\
    frag_tex_coords = one_tex;\n\
    if(tex_on) {\n\
      frag_color = color;\n\
    } else {\n\
      frag_color = color+pos_color*one_color;\n\
    }\n\
  /*frag_color = clamp(frag_color,0,1);*/\n\
\n\
    if(light_on) {\n\
      vec3 _normal = normal+pos_normal*one_normal;\n\
      vec3 frag_normal = vec3(normalize(normal_matrix*vec4(_normal,0)));\n\
      /* at this point, light_direction should be normalized.*/\n\
      float _dot = dot(frag_normal,light_direction);\n\
      if(_dot<0.0) {\n\
        _dot *= -1.0;\n\
        float cooking = 1.4; /*to have same intensity as GL on desktops.*/\n\
        vec4 diffuse_color = light_color*_dot*cooking;\n\
      /*diffuse_color = clamp(diffuse_color,0,1);*/\n\
        vec4 ambient_color = vec4(0,0,0,1);\n\
      /*ambient_color = clamp(ambient_color,0,1);*/\n\
        vec4 _color = (ambient_color+diffuse_color)*frag_color;\n\
        frag_color = vec4(_color.rgb,frag_color.a);\n\
      } else {\n\
        frag_color = vec4(0,0,0,1);\n\
      }\n\
    }\n\
    gl_PointSize = point_size;\n\
    gl_Position = proj_matrix * model_matrix * vec4(one_pos,1);\n\
  }\n\
";
    var vertex_shader = g_gl.createShader(g_gl.VERTEX_SHADER);
    g_gl.shaderSource(vertex_shader,str);
    g_gl.compileShader(vertex_shader);
    if (!g_gl.getShaderParameter(vertex_shader,g_gl.COMPILE_STATUS)) {
      alert(g_gl.getShaderInfoLog(vertex_shader));
    }

    g_shader_program = g_gl.createProgram();
    g_gl.attachShader(g_shader_program,fragment_shader);
    g_gl.attachShader(g_shader_program,vertex_shader);
    g_gl.linkProgram(g_shader_program);
    if (!g_gl.getProgramParameter(g_shader_program,g_gl.LINK_STATUS)) alert('Could not initialise shaders');
  }

  function use_shader_program() {
    g_gl.useProgram(g_shader_program);

    // used by renderer :
    g_sampler = g_gl.getUniformLocation(g_shader_program,'sampler');
    g_tex_on = g_gl.getUniformLocation(g_shader_program,'tex_on');
    g_point_size = g_gl.getUniformLocation(g_shader_program,'point_size');

    g_one_pos = g_gl.getAttribLocation(g_shader_program,'one_pos');
    g_one_normal = g_gl.getAttribLocation(g_shader_program,'one_normal');
    g_one_color = g_gl.getAttribLocation(g_shader_program,'one_color');
    g_one_tex = g_gl.getAttribLocation(g_shader_program,'one_tex');

    g_model_matrix  = g_gl.getUniformLocation(g_shader_program,'model_matrix');
    g_proj_matrix  = g_gl.getUniformLocation(g_shader_program,'proj_matrix');
    g_normal_matrix = g_gl.getUniformLocation(g_shader_program,'normal_matrix');
    g_color = g_gl.getUniformLocation(g_shader_program,'color');
    g_pos_color = g_gl.getUniformLocation(g_shader_program,'pos_color');
    g_normal = g_gl.getUniformLocation(g_shader_program,'normal');
    g_pos_normal = g_gl.getUniformLocation(g_shader_program,'pos_normal');

    g_light_on = g_gl.getUniformLocation(g_shader_program,'light_on');
    g_light_direction = g_gl.getUniformLocation(g_shader_program,'light_direction');
    g_light_color = g_gl.getUniformLocation(g_shader_program,'light_color');

    g_gl.uniform1i(g_tex_on,0);
    g_gl.uniform1f(g_point_size,1);

  //g_gl.uniform4f(g_color,0.5,0.5,0.5,1);
    g_gl.uniform4f(g_color,1,1,1,1);
    g_gl.uniform4f(g_pos_color,0,0,0,0);

    g_gl.uniform3f(g_normal,0,0,1);
    g_gl.uniform3f(g_pos_normal,0,0,0,0);

    g_gl.uniform1i(g_light_on,0);
    g_gl.uniform4f(g_light_color,1,1,1,0);
    g_gl.uniform3f(g_light_direction,0,0,-1);
  }

  /////////////////////////////////////////////////
  /// to shorten C++ side produced code : /////////
  /////////////////////////////////////////////////
  function begdraw(a_x,a_y,a_w,a_h,a_r,a_g,a_b,a_a) {
    //console.log('debug : begdraw : '+a_x+' '+a_y+' '+a_w+' '+a_h+' ');

    //g_gl.enable(g_gl.NORMALIZE);
    //g_gl.shadeModel(g_gl.FLAT);
    ////g_gl.shadeModel(g_gl.GL_SMOOTH);
    //// GL-ES : ::glMaterialfv does not work. We then use :
    ////         ::glEnable(GL_COLOR_MATERIAL) and ::glColor.
    //::glColorMaterial(GL_FRONT, GL_DIFFUSE);  //?
    //g_gl.enable(g_gl.COLOR_MATERIAL);

    // to handle transparency :
    g_gl.disable(g_gl.BLEND);
    g_gl.blendFunc(g_gl.SRC_ALPHA,g_gl.ONE_MINUS_SRC_ALPHA);

    //WARNING : the below enable/disable corresponds
    //          to defaults in sg::state.
    g_gl.clearDepth(1);
    g_gl.enable(g_gl.DEPTH_TEST);
    g_gl.depthFunc(g_gl.LEQUAL);
  //g_gl.disable(LIGHTING);
    g_gl.frontFace(g_gl.CCW);
    g_gl.enable(g_gl.CULL_FACE);
    g_gl.disable(g_gl.POLYGON_OFFSET_FILL);
  //g_gl.disable(g_gl.GL_TEXTURE_2D);

    g_gl.lineWidth(1);
    g_gl.uniform1f(g_point_size,1);

    g_gl.viewport(a_x,a_y,a_w,a_h);
  //g_gl.scissor(a_x,a_y,a_w,a_h);

    g_gl.clearColor(a_r,a_g,a_b,a_a);
    g_gl.clear(g_gl.COLOR_BUFFER_BIT);
    g_gl.clear(g_gl.DEPTH_BUFFER_BIT);

    var ID4 = mat4.create();
    mat4.identity(ID4);
    g_gl.uniformMatrix4fv(g_model_matrix,false,ID4);
    g_gl.uniformMatrix4fv(g_proj_matrix,false,ID4);
    g_gl.uniformMatrix4fv(g_normal_matrix,false,ID4);
  }

  var g_no_atb_zero_warn = true; //to rm warning "Attribute 0...". Seen on Chrome, Firefox.
//var g_no_atb_zero_warn = false;

  function dva(a_mode,a_num,a_data) { //a_data : expect a Float32Array
    var vbo = g_gl.createBuffer();
    g_gl.bindBuffer(g_gl.ARRAY_BUFFER,vbo);
    g_gl.bufferData(g_gl.ARRAY_BUFFER,a_data,g_gl.STATIC_DRAW);
    if(g_no_atb_zero_warn) {
      g_gl.enableVertexAttribArray(g_one_tex);
      g_gl.vertexAttribPointer(g_one_tex,2,g_gl.FLOAT,false,0,0);
    }
    g_gl.enableVertexAttribArray(g_one_pos);
    g_gl.vertexAttribPointer(g_one_pos,3,g_gl.FLOAT,false,0,0);
    g_gl.drawArrays(a_mode,0,a_num); //a_num = nb of points.
    g_gl.disableVertexAttribArray(g_one_pos);
    if(g_no_atb_zero_warn) {
      g_gl.disableVertexAttribArray(g_one_tex);
    }
    g_gl.deleteBuffer(vbo);
  }

  function dvaxy(a_mode,a_num,a_data) { //a_data : expect a Float32Array
    var vbo = g_gl.createBuffer();
    g_gl.bindBuffer(g_gl.ARRAY_BUFFER,vbo);
    g_gl.bufferData(g_gl.ARRAY_BUFFER,a_data,g_gl.STATIC_DRAW);
    if(g_no_atb_zero_warn) {
      g_gl.enableVertexAttribArray(g_one_tex);
      g_gl.vertexAttribPointer(g_one_tex,2,g_gl.FLOAT,false,0,0);
    }
    g_gl.enableVertexAttribArray(g_one_pos);
    g_gl.vertexAttribPointer(g_one_pos,2,g_gl.FLOAT,false,0,0);
    g_gl.drawArrays(a_mode,0,a_num); //a_num = nb of points.
    g_gl.disableVertexAttribArray(g_one_pos);
    if(g_no_atb_zero_warn) {
      g_gl.disableVertexAttribArray(g_one_tex);
    }
    g_gl.deleteBuffer(vbo);
  }

  function dvca(a_mode,a_num,a_data,a_pos_rgbas) {
    g_gl.uniform4f(g_color,0,0,0,0);
    g_gl.uniform4f(g_pos_color,1,1,1,1);
    var vbo = g_gl.createBuffer();
    g_gl.bindBuffer(g_gl.ARRAY_BUFFER,vbo);
    g_gl.bufferData(g_gl.ARRAY_BUFFER,a_data,g_gl.STATIC_DRAW);
    if(g_no_atb_zero_warn) {
      g_gl.enableVertexAttribArray(g_one_tex);
      g_gl.vertexAttribPointer(g_one_tex,2,g_gl.FLOAT,false,0,0);
    }
    g_gl.enableVertexAttribArray(g_one_pos);
    g_gl.vertexAttribPointer(g_one_pos,3,g_gl.FLOAT,false,0,0);
    g_gl.enableVertexAttribArray(g_one_color);
    g_gl.vertexAttribPointer(g_one_color,4,g_gl.FLOAT,false,0,a_pos_rgbas);
    g_gl.drawArrays(a_mode,0,a_num); //a_num = nb of points.
    g_gl.disableVertexAttribArray(g_one_pos);
    g_gl.disableVertexAttribArray(g_one_color);
    if(g_no_atb_zero_warn) {
      g_gl.disableVertexAttribArray(g_one_tex);
    }
    g_gl.deleteBuffer(vbo);
    g_gl.uniform4f(g_pos_color,0,0,0,0);
  }

  function dvna(a_mode,a_num,a_data,a_pos_nms) {
    g_gl.uniform3f(g_normal,0,0,0);
    g_gl.uniform3f(g_pos_normal,1,1,1);
    var vbo = g_gl.createBuffer();
    g_gl.bindBuffer(g_gl.ARRAY_BUFFER,vbo);
    g_gl.bufferData(g_gl.ARRAY_BUFFER,a_data,g_gl.STATIC_DRAW);
    if(g_no_atb_zero_warn) {
      g_gl.enableVertexAttribArray(g_one_tex);
      g_gl.vertexAttribPointer(g_one_tex,2,g_gl.FLOAT,false,0,0);
    }
    g_gl.enableVertexAttribArray(g_one_pos);
    g_gl.vertexAttribPointer(g_one_pos,3,g_gl.FLOAT,false,0,0);
    g_gl.enableVertexAttribArray(g_one_normal);
    g_gl.vertexAttribPointer(g_one_normal,3,g_gl.FLOAT,false,0,a_pos_nms);
    g_gl.drawArrays(a_mode,0,a_num); //a_num = nb of points.
    g_gl.disableVertexAttribArray(g_one_pos);
    g_gl.disableVertexAttribArray(g_one_normal);
    if(g_no_atb_zero_warn) {
      g_gl.disableVertexAttribArray(g_one_tex);
    }
    g_gl.deleteBuffer(vbo);
    g_gl.uniform3f(g_pos_normal,0,0,0);
  }

  function dvnca(a_mode,a_num,a_data,a_pos_nms,a_pos_rgbas) {
    g_gl.uniform4f(g_color,0,0,0,0);
    g_gl.uniform4f(g_pos_color,1,1,1,1);
    g_gl.uniform3f(g_normal,0,0,0);
    g_gl.uniform3f(g_pos_normal,1,1,1);
    var vbo = g_gl.createBuffer();
    g_gl.bindBuffer(g_gl.ARRAY_BUFFER,vbo);
    g_gl.bufferData(g_gl.ARRAY_BUFFER,a_data,g_gl.STATIC_DRAW);
    if(g_no_atb_zero_warn) {
      g_gl.enableVertexAttribArray(g_one_tex);
      g_gl.vertexAttribPointer(g_one_tex,2,g_gl.FLOAT,false,0,0);
    }
    g_gl.enableVertexAttribArray(g_one_pos);
    g_gl.vertexAttribPointer(g_one_pos,3,g_gl.FLOAT,false,0,0);
    g_gl.enableVertexAttribArray(g_one_normal);
    g_gl.vertexAttribPointer(g_one_normal,3,g_gl.FLOAT,false,0,a_pos_nms);
    g_gl.enableVertexAttribArray(g_one_color);
    g_gl.vertexAttribPointer(g_one_color,4,g_gl.FLOAT,false,0,a_pos_rgbas);
    g_gl.drawArrays(a_mode,0,a_num); //a_num = nb of points.
    g_gl.disableVertexAttribArray(g_one_pos);
    g_gl.disableVertexAttribArray(g_one_normal);
    g_gl.disableVertexAttribArray(g_one_color);
    if(g_no_atb_zero_warn) {
      g_gl.disableVertexAttribArray(g_one_tex);
    }
    g_gl.deleteBuffer(vbo);
    g_gl.uniform4f(g_pos_color,0,0,0,0);
    g_gl.uniform3f(g_pos_normal,0,0,0);
  }

  function dvta(a_mode,a_id,a_num,a_data,a_pos_texs) {
    g_gl.uniform1i(g_tex_on,1);
    g_gl.uniform1i(g_sampler,0);
  //g_gl.activeTexture(g_gl.TEXTURE0);
  //g_gl.enable(g_gl.TEXTURE_2D);
    g_gl.bindTexture(g_gl.TEXTURE_2D,a_id);

    var vbo = g_gl.createBuffer();
    g_gl.bindBuffer(g_gl.ARRAY_BUFFER,vbo);
    g_gl.bufferData(g_gl.ARRAY_BUFFER,a_data,g_gl.STATIC_DRAW);

    g_gl.enableVertexAttribArray(g_one_pos);
    g_gl.vertexAttribPointer(g_one_pos,3,g_gl.FLOAT,false,0,0);
    g_gl.enableVertexAttribArray(g_one_tex);
    g_gl.vertexAttribPointer(g_one_tex,2,g_gl.FLOAT,false,0,a_pos_texs);
    g_gl.drawArrays(a_mode,0,a_num);
    g_gl.disableVertexAttribArray(g_one_pos);
    g_gl.disableVertexAttribArray(g_one_tex);
    g_gl.deleteBuffer(vbo);

    g_gl.uniform1i(g_tex_on,0);
  }

  function dvnta(a_mode,a_id,a_num,a_data,a_pos_nms,a_pos_texs) {
    g_gl.uniform1i(g_tex_on,1);
    g_gl.uniform1i(g_sampler,0);
    g_gl.uniform3f(g_normal,0,0,0);
    g_gl.uniform3f(g_pos_normal,1,1,1);
  //g_gl.activeTexture(g_gl.TEXTURE0);
  //g_gl.enable(g_gl.TEXTURE_2D);
    g_gl.bindTexture(g_gl.TEXTURE_2D,a_id);

    var vbo = g_gl.createBuffer();
    g_gl.bindBuffer(g_gl.ARRAY_BUFFER,vbo);
    g_gl.bufferData(g_gl.ARRAY_BUFFER,a_data,g_gl.STATIC_DRAW);

    g_gl.enableVertexAttribArray(g_one_pos);
    g_gl.enableVertexAttribArray(g_one_normal);
    g_gl.enableVertexAttribArray(g_one_tex);
    g_gl.vertexAttribPointer(g_one_pos,3,g_gl.FLOAT,false,0,0);
    g_gl.vertexAttribPointer(g_one_normal,3,g_gl.FLOAT,false,0,a_pos_nms);
    g_gl.vertexAttribPointer(g_one_tex,2,g_gl.FLOAT,false,0,a_pos_texs);
    g_gl.drawArrays(a_mode,0,a_num);
    g_gl.disableVertexAttribArray(g_one_pos);
    g_gl.disableVertexAttribArray(g_one_normal);
    g_gl.disableVertexAttribArray(g_one_tex);
    g_gl.deleteBuffer(vbo);

    g_gl.uniform1i(g_tex_on,0);
    g_gl.uniform3f(g_pos_normal,0,0,0);
  }

  function ctex() {return g_gl.createTexture();}
  function cbuf() {return g_gl.createBuffer();}

  function dtex(a_id) {
    if(typeof a_id != 'undefined' ) {
      g_gl.deleteTexture(a_id);
      delete a_id;
    }
  }
  function dbuf(a_id) {
    if(typeof a_id != 'undefined' ) {
      g_gl.deleteBuffer(a_id);
      delete a_id;
    }
  }

  function bbd(a_id,a_data) {
    g_gl.bindBuffer(g_gl.ARRAY_BUFFER,a_id);
    g_gl.bufferData(g_gl.ARRAY_BUFFER,a_data,g_gl.STATIC_DRAW);
    //g_gl.bindBuffer(g_gl.ARRAY_BUFFER,0);
  }

  function bb(a_id) {g_gl.bindBuffer(g_gl.ARRAY_BUFFER,a_id);}

  function dvb(a_mode,a_num,a_pos_xyzs) {
    if(g_no_atb_zero_warn) {
      g_gl.enableVertexAttribArray(g_one_tex);
      g_gl.vertexAttribPointer(g_one_tex,2,g_gl.FLOAT,false,0,0);
    }
    g_gl.enableVertexAttribArray(g_one_pos);
    g_gl.vertexAttribPointer(g_one_pos,3,g_gl.FLOAT,false,0,a_pos_xyzs);
    g_gl.drawArrays(a_mode,0,a_num);
    g_gl.disableVertexAttribArray(g_one_pos);
    if(g_no_atb_zero_warn) {
      g_gl.disableVertexAttribArray(g_one_tex);
    }
  }

  function dvcb(a_mode,a_num,a_pos_xyzs,a_pos_rgbas) {
    g_gl.uniform4f(g_color,0,0,0,0);
    g_gl.uniform4f(g_pos_color,1,1,1,1);
    if(g_no_atb_zero_warn) {
      g_gl.enableVertexAttribArray(g_one_tex);
      g_gl.vertexAttribPointer(g_one_tex,2,g_gl.FLOAT,false,0,0);
    }
    g_gl.enableVertexAttribArray(g_one_pos);
    g_gl.vertexAttribPointer(g_one_pos,3,g_gl.FLOAT,false,0,a_pos_xyzs);
    g_gl.enableVertexAttribArray(g_one_color);
    g_gl.vertexAttribPointer(g_one_color,4,g_gl.FLOAT,false,0,a_pos_rgbas);
    g_gl.drawArrays(a_mode,0,a_num);
    g_gl.disableVertexAttribArray(g_one_pos);
    g_gl.disableVertexAttribArray(g_one_color);
    if(g_no_atb_zero_warn) {
      g_gl.disableVertexAttribArray(g_one_tex);
    }
    g_gl.uniform4f(g_pos_color,0,0,0,0);
  }

  function dvnb(a_mode,a_num,a_pos_xyzs,a_pos_nms) {
    g_gl.uniform3f(g_normal,0,0,0);
    g_gl.uniform3f(g_pos_normal,1,1,1);
    if(g_no_atb_zero_warn) {
      g_gl.enableVertexAttribArray(g_one_tex);
      g_gl.vertexAttribPointer(g_one_tex,2,g_gl.FLOAT,false,0,0);
    }
    g_gl.enableVertexAttribArray(g_one_pos);
    g_gl.vertexAttribPointer(g_one_pos,3,g_gl.FLOAT,false,0,a_pos_xyzs);
    g_gl.enableVertexAttribArray(g_one_normal);
    g_gl.vertexAttribPointer(g_one_normal,3,g_gl.FLOAT,false,0,a_pos_nms);
    g_gl.drawArrays(a_mode,0,a_num);
    g_gl.disableVertexAttribArray(g_one_pos);
    g_gl.disableVertexAttribArray(g_one_normal);
    if(g_no_atb_zero_warn) {
      g_gl.disableVertexAttribArray(g_one_tex);
    }
    g_gl.uniform3f(g_pos_normal,0,0,0);
  }

  function dvncb(a_mode,a_num,a_pos_xyzs,a_pos_nms,a_pos_rgbas) {
    g_gl.uniform4f(g_color,0,0,0,0);
    g_gl.uniform4f(g_pos_color,1,1,1,1);
    g_gl.uniform3f(g_normal,0,0,0);
    g_gl.uniform3f(g_pos_normal,1,1,1);
    if(g_no_atb_zero_warn) {
      g_gl.enableVertexAttribArray(g_one_tex);
      g_gl.vertexAttribPointer(g_one_tex,2,g_gl.FLOAT,false,0,0);
    }
    g_gl.enableVertexAttribArray(g_one_pos);
    g_gl.vertexAttribPointer(g_one_pos,3,g_gl.FLOAT,false,0,a_pos_xyzs);
    g_gl.enableVertexAttribArray(g_one_normal);
    g_gl.vertexAttribPointer(g_one_normal,3,g_gl.FLOAT,false,0,a_pos_nms);
    g_gl.enableVertexAttribArray(g_one_color);
    g_gl.vertexAttribPointer(g_one_color,4,g_gl.FLOAT,false,0,a_pos_rgbas);
    g_gl.drawArrays(a_mode,0,a_num);
    g_gl.disableVertexAttribArray(g_one_pos);
    g_gl.disableVertexAttribArray(g_one_normal);
    g_gl.disableVertexAttribArray(g_one_color);
    if(g_no_atb_zero_warn) {
      g_gl.disableVertexAttribArray(g_one_tex);
    }
    g_gl.uniform4f(g_pos_color,0,0,0,0);
    g_gl.uniform3f(g_pos_normal,0,0,0);
  }

  function ldcol(a_r,a_g,a_b,a_a) {g_gl.uniform4f(g_color,a_r,a_g,a_b,a_a);}
  function ldnormal(a_x,a_y,a_z) {g_gl.uniform3f(g_normal,a_x,a_y,a_z);}

  //function gon(a_v) {g_gl.enable(a_v);}
  //function goff(a_v) {g_gl.disable(a_v);}
  function glw(a_v) {g_gl.lineWidth(a_v);}
  function gffcw() {g_gl.frontFace(g_gl.CW);}
  function gffccw() {g_gl.frontFace(g_gl.CCW);}

  function gff(a_v) {
    if(a_v) {
      g_gl.frontFace(g_gl.CCW);
    } else {
      g_gl.frontFace(g_gl.CW);
    }
  }

  function gps(a_v) {g_gl.uniform1f(g_point_size,a_v);}

  // code must match inlib/glprims :
  function md0() {return g_gl.POINTS;}
  function md1() {return g_gl.LINES;}
  function md2() {return g_gl.LINE_LOOP;}
  function md3() {return g_gl.LINE_STRIP;}
  function md4() {return g_gl.TRIANGLES;}
  function md5() {return g_gl.TRIANGLE_STRIP;}
  function md6() {return g_gl.TRIANGLE_FAN;}

  function mode(a_value) {  //must match inlib/glprims.
    if(a_value==0) return g_gl.POINTS;
    if(a_value==1) return g_gl.LINES;
    if(a_value==2) return g_gl.LINE_LOOP;
    if(a_value==3) return g_gl.LINE_STRIP;
    if(a_value==4) return g_gl.TRIANGLES;
    if(a_value==5) return g_gl.TRIANGLE_STRIP;
    if(a_value==6) return g_gl.TRIANGLE_FAN;
    return g_gl.POINTS;
  }

  function lgset(a_x,a_y,a_z,a_r,a_g,a_b,a_a) {
    g_gl.uniform1i(g_light_on,1);
    g_gl.uniform3f(g_light_direction,a_x,a_y,a_z);
    g_gl.uniform4f(g_light_color,a_r,a_g,a_b,a_a);
  }
  function lgon() {g_gl.uniform1i(g_light_on,1);}
  function lgoff() {g_gl.uniform1i(g_light_on,0);}

  function lg(a_v) {g_gl.uniform1i(g_light_on,a_v);}

  function gpo(a_v) {
    if(a_v) g_gl.enable(g_gl.POLYGON_OFFSET_FILL);
    else    g_gl.disable(g_gl.POLYGON_OFFSET_FILL);
    if(a_v) g_gl.polygonOffset(1.,1.);
  }
  function gdt(a_v) {
    if(a_v) g_gl.enable(g_gl.DEPTH_TEST);
    else    g_gl.disable(g_gl.DEPTH_TEST);
  }
  function gcf(a_v) {
    if(a_v) g_gl.enable(g_gl.CULL_FACE);
    else    g_gl.disable(g_gl.CULL_FACE);
  }
  function gbd(a_v) {
    if(a_v) g_gl.enable(g_gl.BLEND);
    else    g_gl.disable(g_gl.BLEND);
  }

/*
  function ldtexi(a_texture,a_image) {
    g_gl.pixelStorei(g_gl.UNPACK_FLIP_Y_WEBGL,true);
    g_gl.bindTexture(g_gl.TEXTURE_2D,a_texture);
    g_gl.texImage2D(g_gl.TEXTURE_2D,0,g_gl.RGBA,g_gl.RGBA,g_gl.UNSIGNED_BYTE,a_image);
    g_gl.texParameteri(g_gl.TEXTURE_2D,g_gl.TEXTURE_MAG_FILTER,g_gl.NEAREST);
    g_gl.texParameteri(g_gl.TEXTURE_2D,g_gl.TEXTURE_MIN_FILTER,g_gl.NEAREST);
    g_gl.bindTexture(g_gl.TEXTURE_2D,null);
  }
*/

  function ldtexb(a_texture,a_w,a_h,a_data) {
  //g_gl.pixelStorei(g_gl.UNPACK_FLIP_Y_WEBGL,true);
    g_gl.bindTexture(g_gl.TEXTURE_2D,a_texture);
    g_gl.texImage2D(g_gl.TEXTURE_2D,0,g_gl.RGB,a_w,a_h,0,g_gl.RGB,g_gl.UNSIGNED_BYTE,a_data);
    g_gl.texParameteri(g_gl.TEXTURE_2D,g_gl.TEXTURE_MAG_FILTER,g_gl.NEAREST);
    g_gl.texParameteri(g_gl.TEXTURE_2D,g_gl.TEXTURE_MIN_FILTER,g_gl.NEAREST);
    g_gl.bindTexture(g_gl.TEXTURE_2D,null);
  }

  function ldtexba(a_texture,a_w,a_h,a_data) {
  //g_gl.pixelStorei(g_gl.UNPACK_FLIP_Y_WEBGL,true);
    g_gl.bindTexture(g_gl.TEXTURE_2D,a_texture);
    // a_data is expected to be a Uint8Array containing the image.
    g_gl.texImage2D(g_gl.TEXTURE_2D,0,g_gl.RGBA,a_w,a_h,0,g_gl.RGBA,g_gl.UNSIGNED_BYTE,a_data);
    g_gl.texParameteri(g_gl.TEXTURE_2D,g_gl.TEXTURE_MAG_FILTER,g_gl.NEAREST);
    g_gl.texParameteri(g_gl.TEXTURE_2D,g_gl.TEXTURE_MIN_FILTER,g_gl.NEAREST);
    g_gl.bindTexture(g_gl.TEXTURE_2D,null);
  }

  function ldmatp(a_vals) {
    g_gl.uniformMatrix4fv(g_proj_matrix,false,mat4.create(a_vals));
  }

  function ldmatm(a_vals) {
    g_gl.uniformMatrix4fv(g_model_matrix,false,mat4.create(a_vals));
  }
  function ldmatn(a_vals) {
    g_gl.uniformMatrix4fv(g_normal_matrix,false,mat4.create(a_vals));
  }

  function get_rgbas(a_w,a_h) {  //gl.drawingBufferWidth,gl.drawingBufferHeight
    var pixels = new Uint8Array(a_w*a_h*4);
    g_gl.readPixels(0,0,a_w,a_h,g_gl.RGBA,g_gl.UNSIGNED_BYTE,pixels);
    return pixels;
  }

  /////////////////////////////////////////////////
  /////////////////////////////////////////////////
  /////////////////////////////////////////////////
  function set_canvas_cursor(a_style) {
    document.getElementById('gl_canvas').style.cursor = a_style;
  }

  /////////////////////////////////////////////////
  /////////////////////////////////////////////////
  /////////////////////////////////////////////////
/*
  var g_float_size = 0
  var g_float_buffer = null;

  function set_float_buffer(a_buffer,a_size) {
    if((g_float_buffer==null)||(a_size>g_float_size)) {
      console.log('debug : set_float_buffer '+a_size);
      g_float_buffer = new Float32Array(HEAPF32.buffer,a_buffer,a_size);
      g_float_size = a_size;
    }
  }
*/
