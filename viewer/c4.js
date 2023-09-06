config = {
    mode: {options:["Explore", "Play"], select:0, key:'M'},
    solutions: {options:["Invisible", "Visible"], select:0, key:'Q'},
    path: {options:["Invisible", "Visible"], select:0, key:'P'},
};
const game_blurb = [
    "This is a complete weak solution of a certain connect 4 position.",
    "The right depicts the graph of all positions in that weak solution.",
    "This is (close to) the minimum amount of information which you need to",
    "'memorize' in order to be able to always win starting from the shown position.",
    "Terminal nodes contain 'steady state diagrams', a language which expresses",
    "low-information weak solutions of sufficiently developed positions."
]
var EMPTY       = "#049";
var RED         = "#f00";
var YELLOW      = "#ff0";










function get_color(name, neighbor_name){
    return Math.min(nodes[name].representation.length, nodes[neighbor_name].representation.length)%2==1?YELLOW:RED
}

let board_arr = 0;

var square_sz = 40;

function get_hash() {
    console.log(board_arr)
    let a = 1;
    let hash_in_progress = 0;

    for (let i = 0; i < parsedData.board_h; i++) {
        for (let j = 0; j < parsedData.board_w; j++) {
            hash_in_progress += board_arr[parsedData.board_h-1-i][j] * a;
            a *= 1.21813947;
        }
    }

    a = 1;
    let hash_in_progress_2 = 0;

    for (let i = 0; i < parsedData.board_h; i++) {
        for (let j = 0; j < parsedData.board_w; j++) {
            hash_in_progress_2 += board_arr[parsedData.board_h-1-i][parsedData.board_w - 1 - j] * a;
            a *= 1.21813947;
        }
    }

    let semihash = hash_in_progress * hash_in_progress_2;

    var closedist = 1;
    var closename = -1;
    for(name in nodes){
        var dist = Math.abs(semihash-name);
        if(dist < closedist){
            closename = name;
            closedist = dist;
        }
    }
    return closename;
}

function render_board () {
    boardcanvas.width = (parseInt(parsedData.board_w))*square_sz;
    boardcanvas.height = (parseInt(parsedData.board_h))*square_sz;
    boardctx.font = "24px Arial";
    boardctx.textAlign = "center";
    board_arr = [];
    for(let y=0; y<6; y++) {
        board_arr[y] = [];
        for(let x=0; x<7; x++) {
            board_arr[y][x] = 0;
        }
    }
    var board_string = nodes[hash].representation;
    for (var i = 0; i < board_string.length; i++){
        var x = String.fromCharCode(board_string.charCodeAt(i))-1;

        // place the piece
        for (var y=0; y<6; y++) {
            if(board_arr[y][x] === 0){
                board_arr[y][x] = i%2 + 1;
                break;
            }
        }
    }
    for(var x = 0; x < 7; x++)
        for(var y = 0; y < 6; y++)
            drawStone(x, y, board_arr[y][x]);
}

function drawStone(x, y, col) {
    col = ["#026", "#900", "#760"][col];
    var px = (x+.5)*square_sz;
    var py = (5-y+.5)*square_sz;

    boardctx.fillStyle = col;
    boardctx.beginPath();
    boardctx.arc(px, py, 18, 0, 2*Math.PI, false);
    boardctx.fill();
    var ss = String.fromCharCode(nodes[hash].data.steadystate[5-y][x]);
    if(ss == '1' || ss == '2') return;
    boardctx.fillStyle = "white";
    boardctx.fillText(ss, px, py+9);
}

boardcanvas.addEventListener('click', handleClick);

function handleClick(event) {
    // Get the mouse click coordinates relative to the canvas
    const rect = boardcanvas.getBoundingClientRect();
    const mouseX = event.clientX - rect.left;
    const mouseY = event.clientY - rect.top;

    // Calculate the column where the player clicked
    const column = Math.floor(mouseX / square_sz);

    console.log(board_arr)
    // Call a function to handle the player's move in this column
    makeMove(column + 1);
}

setInterval(makeMoveAsRed, 300);

function makeMoveAsRed(){
    if(nodes[hash].representation.length%2 == 1) return;
    for(var neighbor_id in nodes[hash].neighbors){
        var neighbor_hash = nodes[hash].neighbors[neighbor_id];
        var neighbor = nodes[neighbor_hash];
        if(neighbor.representation.length > nodes[hash].representation.length){
            hash = neighbor_hash;
            on_board_change();
            return;
        }
    }
}

function makeMove(column) {
    let x = column-1;

    let who = nodes[hash].representation.length%2 + 1

    // place the piece
    for (var y=0; y<6; y++) {
        if(board_arr[y][x] === 0){
            board_arr[y][x] = who;
            break;
        }
        if(y == 5) return false;
    }
    newhash = get_hash();
    if(newhash != -1) hash = newhash;
    on_board_change();
    return newhash != -1;
}
