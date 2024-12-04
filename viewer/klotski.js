config = {
    colors: {options:["Distance from Start", "Distance from Solution", "Off"], select:0, key:'C'},
    solutions: {options:["Invisible", "Visible"], select:0, key:'Q'},
    path: {options:["Invisible", "Visible"], select:0, key:'P'},
};
const game_blurb = [
    "Slide the blocks in the puzzle!",
    "The right depicts the graph of all positions of the puzzle."
]

var EMPTY_SPACE = '.';






function normsin(angle){
    return Math.floor(128.0*(Math.sin(angle)+1));
}

function color_wheel(angle, brightness=1){
    angle*=0.025;
    var r = normsin(angle)*brightness;
    var g = normsin(angle+Math.PI*2.0/3)*brightness;
    var b = normsin(angle+Math.PI*4.0/3)*brightness;
    return "rgb("+r+","+g+","+b+")";
}

function get_color(name, neighbor_name){
    if(config.colors.select == 0) return color_wheel(nodes[name].dist);
    else if(config.colors.select == 1) return color_wheel(nodes[name].solution_dist);
    else return "gray";
}

var square_sz = 40;

function on_click_node(){}

function render_board () {
    boardcanvas.width = (parseInt(parsedData.board_w) + 1) * square_sz;
    boardcanvas.height = (parseInt(parsedData.board_h) + 1) * square_sz;
    for (i = 0; i < 3; i++) {
        boardctx.fillStyle = ([`#000`, `#222`, `#000`])[i];
        var margin = i * square_sz / 5;
        boardctx.fillRect(0 + margin, 0 + margin, boardcanvas.width - 2 * margin, boardcanvas.height - 2 * margin);
    }
    for (var y = 0; y < parsedData.board_h; y++) {
        for (var x = 0; x < parsedData.board_w; x++) {
            var spot = y * parsedData.board_w + x;
            var character = nodes[hash].representation.charAt(spot);
            if (character == EMPTY_SPACE) continue;
            boardctx.fillStyle = "white";
            var conddiff = (character == board_click_square && can_move_piece(Math.sign(diffcoords.y), Math.sign(diffcoords.x))) ? diffcoords : { x: 0, y: 0 };
            
            // Check if the left and lower two nodes have the same charcode
            if (x < parsedData.board_w - 1 &&
                nodes[hash].representation.charAt(spot + 1) == character) {
                // Render a wide rectangle to connect the two cells
                boardctx.fillRect(boardcanvas.width / 2 + (x - parsedData.board_w / 2) * square_sz + conddiff.x + 2,
                                  boardcanvas.height / 2 + (y - parsedData.board_h / 2) * square_sz + conddiff.y + 2,
                                  square_sz * 2 - 4,
                                  square_sz - 4);
            }
            if (y < parsedData.board_h - 1 &&
                nodes[hash].representation.charAt(spot + parsedData.board_w) == character) {
                // Render a wide rectangle to connect the two cells
                boardctx.fillRect(boardcanvas.width / 2 + (x - parsedData.board_w / 2) * square_sz + conddiff.x + 2,
                                  boardcanvas.height / 2 + (y - parsedData.board_h / 2) * square_sz + conddiff.y + 2,
                                  square_sz - 4,
                                  square_sz * 2 - 4);
            }
            // Render a normal rectangle
            boardctx.fillRect(boardcanvas.width / 2 + (x - parsedData.board_w / 2) * square_sz + conddiff.x + 2,
                              boardcanvas.height / 2 + (y - parsedData.board_h / 2) * square_sz + conddiff.y + 2,
                              square_sz - 4,
                              square_sz - 4);
        }
    }
}

function in_bounds(min, val, max){ return min <= val && val < max; }

function can_move_piece(dy, dx){
    if(nodes[hash].data.rushhour && (board_click_square.charCodeAt(0) - 'a'.charCodeAt(0) + dy)%2==0) return false;
    for(var y = 0; y < parsedData.board_h; y++)
        for(var x = 0; x < parsedData.board_w; x++){
            if(nodes[hash].representation.charAt(y*parsedData.board_w+x) == board_click_square) {
                var inside = in_bounds(0, y+dy, parsedData.board_h) && in_bounds(0, x+dx, parsedData.board_w);
                var target = nodes[hash].representation.charAt((y+dy)*parsedData.board_w+(x+dx));
                if(!inside || (target != EMPTY_SPACE && target != board_click_square))
                    return false;
            }
            else continue;
        }
    return true;
}

function move_piece(dy, dx){
    var board_string_new = '';
    for(var i = 0; i < parsedData.board_h*parsedData.board_w; i++)board_string_new += ".";
    for(var y = 0; y < parsedData.board_h; y++)
        for(var x = 0; x < parsedData.board_w; x++){
            var position = y*parsedData.board_w+x;
            var letter_here = nodes[hash].representation.charAt(position);
            if(letter_here == board_click_square) {
                var target = (y+dy)*parsedData.board_w+(x+dx);
                board_string_new = board_string_new.slice(0, target) + board_click_square + board_string_new.slice(target+1);
            }
            else if(letter_here != EMPTY_SPACE)
                board_string_new = board_string_new.slice(0, position) + letter_here + board_string_new.slice(position+1);
        }
    hash = get_hash(board_string_new);
}

var board_release = function(){
    boardbutton = false;
    var dx = Math.sign(diffcoords.x);
    var dy = Math.sign(diffcoords.y);
    if(can_move_piece(dy, dx)) move_piece(dy, dx);
    board_click_start = {x:0,y:0};
    diffcoords = {x:0,y:0};
    on_board_change();
}

boardcanvas.addEventListener(`mousedown`, function(e){
    boardbutton = true;
    var rect = boardcanvas.getBoundingClientRect();
    board_click_start = {
        x: e.clientX - rect.left,
        y: e.clientY - rect.top
    };
    var x = Math.floor(((board_click_start.x - boardcanvas.width/2)/square_sz)+parsedData.board_w/2);
    var y = Math.floor(((board_click_start.y - boardcanvas.height/2)/square_sz)+parsedData.board_h/2);
    diffcoords = {x:0,y:0};
    board_click_square = nodes[hash].representation.charAt(x+y*parsedData.board_w);
    if(!(in_bounds(0, y, parsedData.board_h) && in_bounds(0, x, parsedData.board_w))) board_click_square = ';';
}, false);
boardcanvas.addEventListener(`mouseup`, board_release, false);
boardcanvas.addEventListener(`mouseleave`, board_release, false);
boardcanvas.addEventListener(`mousemove`, function(e){
    if(!boardbutton) return;
    var rect = boardcanvas.getBoundingClientRect();
    var screen_coords = {
        x: e.clientX - rect.left,
        y: e.clientY - rect.top
    };
    diffcoords = {
        x:screen_coords.x-board_click_start.x,
        y:screen_coords.y-board_click_start.y
    };
    if(Math.abs(diffcoords.x) > Math.abs(diffcoords.y))
        diffcoords.y = 0;
    else
        diffcoords.x = 0;
    diffcoords.x = Math.min(square_sz, Math.max(-square_sz, diffcoords.x));
    diffcoords.y = Math.min(square_sz, Math.max(-square_sz, diffcoords.y));
}, false);

function get_hash(rep) {
    var semihash = 0;
    var obj = {"a":0, "b":0, "c":0, "d":0, "e":0, "f":0, "g":0, "h":0, "i":0, "j":0, "k":0, "l":0, "m":0, "n":0, "o":0, "p":0, "q":0, "r":0, "s":0, "t":0, "u":0, "v":0, "w":0, "x":0, "y":0, "z":0}
    for(var y = 0; y < parsedData.board_h; y++)
        for(var x = 0; x < parsedData.board_w; x++){
            var letter = rep.charAt(y*parsedData.board_w+x);
            if(letter != EMPTY_SPACE){
                var i=y*parsedData.board_w+x;
                obj[letter] += Math.sin((i+1)*Math.cbrt(i+2));
            }
        }
    for(letter in obj) semihash+=Math.cbrt(obj[letter]);

    var closedist = 10000;
    var closename = 0;
    for(name in nodes){
        var dist = Math.abs(semihash-name);
        if(dist < closedist){
            closename = name;
            closedist = dist;
        }
    }
    return closename;
}

setInterval(render_board, 15);

/*function render_histogram(){
    var l = parsedData.histogram_solutions.length;
    var max = 0;
    for(var i = 0; i < l; i++){
        var hns= parsedData.histogram_non_solutions[i];
        var hs = parsedData.histogram_solutions[i];
        max = Math.max(max, hns+hs);
    }

    graphctx.globalAlpha = 1;
    for(var i = 0; i < l+1; i++){
        var hns= parsedData.histogram_non_solutions[i];
        var hs = parsedData.histogram_solutions[i];
        var dark = i == nodes[hash].dist?.5:1;
        graphctx.fillStyle = color_wheel(i, dark);
        var bar_width = (hns+hs)*300/max;
        graphctx.fillRect(w-bar_width, h*i/l, bar_width, h/l+1);

        hs = parsedData.histogram_solutions[i-1];
        dark = i-1 == nodes[hash].dist?.25:.5;
        graphctx.fillStyle = color_wheel(i-1, dark);
        bar_width = hs*300/max;
        graphctx.fillRect(w-bar_width, h*(i-1)/l, bar_width, h/l+1);
    }
}*/
