
var g_to_open = false;
var g_curr_line = '';
var g_anim_timer_id = null;
var g_event_anim_timer_id = null;
var g_mouse_down = 0;


function canvas_resize() {
  // Arrange so that the GL canvas be visible in the window by
  // mapping the whole window area. This has the same behaviour as native apps.
  var h = window.innerHeight;
  var w = window.innerWidth;
  const canvas = document.getElementById('gl_canvas');
  canvas.width = w;
  canvas.height = h;
}

function window_resize() {
  canvas_resize();
  const canvas = document.getElementById('gl_canvas');
  Module.app_set_size(canvas.width, canvas.height);
  Module.app_win_render();
}

function window_keydown(a_event) {
//  console.log(a_event);
  if(a_event.code=='Enter') {
    console.log('enter');
  } else if(a_event.code=='ArrowUp') {
    console.log('up');
  } else if(a_event.code=='ArrowDown') {
    console.log('down');
  } else if(a_event.code=='ArrowRight') {
    console.log('right');
  } else if(a_event.code=='ArrowLeft') {
    console.log('left');
  } else if(a_event.code=='Backspace') {
    console.log('back');
  } else if( (a_event.keyCode>=32)) {
    g_curr_line += a_event.key;
    console.log(g_curr_line);
  }
}

function anim_proc() {
  if(Module.app_do_works()) Module.app_win_render();
  //console.log('debug : anim_proc');
}

function handle_button_down(a_event) {
  if(a_event.button==0) {
    g_mouse_down = 1;
    a_event.stopPropagation();
    a_event.preventDefault();
    Module.app_button_down(a_event.button,a_event.offsetX,a_event.offsetY);
  }
}

function handle_button_up(a_event) {
  if(a_event.button==0) {
    g_mouse_down = 0;
    a_event.stopPropagation();
    a_event.preventDefault();
    Module.app_button_up(a_event.button,a_event.offsetX,a_event.offsetY);
  }
}

function handle_button_move(a_event) {
  if( (a_event.button==0) && (g_mouse_down==1)) {
    a_event.stopPropagation();
    a_event.preventDefault();
    Module.app_button_move(a_event.button,a_event.offsetX,a_event.offsetY);
  }
}

function handle_touch_start(a_event) {
  a_event.stopPropagation();
  a_event.preventDefault();
  var touches = a_event.changedTouches;
  if(!touches.length) return;
  g_mouse_down = 1;
  var x = touches[0].pageX;
  var y = touches[0].pageY;
  Module.app_button_down(0,x,y);
}

function handle_touch_end(a_event) {
  a_event.stopPropagation();
  a_event.preventDefault();
  var touches = a_event.changedTouches;
  if(!touches.length) return;
  g_mouse_down = 0;
  var x = touches[0].pageX;
  var y = touches[0].pageY;
  Module.app_button_up(0,x,y);
}

function handle_touch_move(a_event) {
  a_event.stopPropagation();
  a_event.preventDefault();
  var touches = a_event.changedTouches;
  if(!touches.length) return;
  if(g_mouse_down==1) {
    var x = touches[0].pageX;
    var y = touches[0].pageY;
    Module.app_button_move(0,x,y);
  }
}

function handle_drag_over(a_event) {
  a_event.stopPropagation();
  a_event.preventDefault();
  a_event.dataTransfer.dropEffect = 'copy';
}

function upload_first_file(a_files,a_open,a_warn) {
  if(a_files && (a_files.length === 1)) {
    var file = a_files[0];
    if(file.name) {
      const file_name = file.name;
      const file_reader = new FileReader();
      file_reader.onload = (a_event) => {
        upload_file(file_name,new Int8Array(a_event.target.result),a_warn);
        if(a_open==true) Module.app_open_doc_file(file_name,'');
      };
      file_reader.onerror = () => {console.error('Unable to read file ' + file_name + '.');};
      file_reader.readAsArrayBuffer(file);
    } else {
      alert('file.name does not exist.');
    }
  }
}

function upload_file(a_file_name,a_data,a_warn) {
  const start_time = new Date();
  const num_bytes = a_data.length * a_data.BYTES_PER_ELEMENT;
  const data_ptr = Module._malloc(num_bytes);
  const data_on_heap = new Int8Array(Module.HEAP8.buffer, data_ptr, num_bytes);
  data_on_heap.set(a_data);
  Module.app_upload_file(a_file_name,data_on_heap.byteOffset,num_bytes,a_warn);
  Module._free(data_ptr);
  //console.log('[Copy to Heap (Cwrap)] Time to load: ' + (new Date() - start_time));
}

function upload_buffer(a_data,a_sargs) {
  const start_time = new Date();
  const num_bytes = a_data.length * a_data.BYTES_PER_ELEMENT;
  const data_ptr = Module._malloc(num_bytes);
  const data_on_heap = new Int8Array(Module.HEAP8.buffer, data_ptr, num_bytes);
  data_on_heap.set(a_data);
  Module.app_upload_buffer(data_on_heap.byteOffset,num_bytes,a_sargs);
  Module._free(data_ptr);
  //console.log('[Copy to Heap (Cwrap)] Time to load: ' + (new Date() - start_time));
}

function handle_drop(a_event) {
  a_event.stopPropagation();
  a_event.preventDefault();
  upload_first_file(a_event.dataTransfer.files,true);
}

function download_app_doc_file(a_file_name) {
  if(a_file_name.length==0) return;
  var length = Module.app_get_doc_file_size(a_file_name);
  if(length==(-1)) {alert('File not found.');return;}
  var buffer = Module.app_get_doc_file(a_file_name);
  const array = new Int8Array(Module.HEAP8.buffer,buffer,length);
  download(array,a_file_name,'example/binary');
  Module.app_delete_buffer(buffer);
}

function open_file_chooser() {g_to_open=false;document.getElementById('app_input_file').click();}
function open_file_chooser_open() {g_to_open=true;document.getElementById('app_input_file').click();}

function run_time_init(a_canvas) {
  if(Module.app_initialize(g_app_name,a_canvas.width,a_canvas.height)==0) {
    console.log('main.js/run_time_init : Module.app_initialize() failed.');
    return;
  }
  a_canvas.addEventListener('mousedown' ,handle_button_down);
  a_canvas.addEventListener('mouseup'   ,handle_button_up);
  a_canvas.addEventListener('mousemove' ,handle_button_move);
  document.addEventListener('dragover'  ,handle_drag_over);
  document.addEventListener('drop'      ,handle_drop);

  a_canvas.addEventListener('touchstart',handle_touch_start);
  a_canvas.addEventListener('touchend'  ,handle_touch_end);
  a_canvas.addEventListener('touchmove' ,handle_touch_move);

  const gui_area = document.getElementById('gui_area');

  //this widget is rather ugly, we hide it and trigger it with the below button
 {var elem = document.createElement('input');
  gui_area.appendChild(elem);
  elem.type = 'file';
  elem.id = 'app_input_file';
  elem.style.width = '300px';
  elem.style.height = '45px';
  elem.style.background = 'white';
  elem.style.color = 'black';
  elem.style.fontSize = '20px';
  elem.style.borderWidth = '2px';
  elem.style.borderColor = 'black';
  elem.style.boxShadow = 'none';
  elem.style.display = 'none';
  elem.onchange = (a_event) => {
    upload_first_file(a_event.target.files,g_to_open,true);
  }}
    
}
