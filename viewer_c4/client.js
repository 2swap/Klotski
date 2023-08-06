const graphcanvas = document.getElementById(`graph`);
const w = graphcanvas.width = window.innerWidth;
const h = graphcanvas.height = window.innerHeight;
const graphctx = graphcanvas.getContext(`2d`);

let tick = 0;
let ox = 0; let oy = 100; let zoom = 1;
let alpha = 0;
let graphbutton = false;
//var save_start_board = board_string;

var config = {
    blurbs: {options:["Invisible", "Visible"], select:0},
};

$(document).ready(async function() {
    try {
        const response = await fetch('data.json');
        if (!response.ok) {
            throw new Error('Failed to fetch JSON');
        }

        const parsedData = await response.json();
        // Access different properties from the parsed data
        var blurb = parsedData.blurb;
        var board_h = parsedData.board_h;
        var board_w = parsedData.board_w;
        var board_string = parsedData.board_string;
        var histogram_non_solutions = parsedData.histogram_non_solutions;
        var histogram_solutions = parsedData.histogram_solutions;
        var nodes_to_use = parsedData.nodes_to_use;
        var rushhour = parsedData.rushhour;

        let nodes = {};

        increment_max();

        function increment_max(){
            var max_x = 0;
            for (const name in nodes_to_use){
                max_x = Math.max(nodes_to_use[name].x, max_x);
            }
            for (const name in nodes_to_use){
                node = nodes_to_use[name];
                hash = name;
                nodes[name] = node;
                node.x*=w*.2/max_x;
                node.y*=w*.2/max_x;
                node.z*=w*.2/max_x;
                delete nodes_to_use[name];
            }
        }

        var EMPTY       = "#049";
        var RED         = "#f00";
        var YELLOW      = "#ff0";

        function render_graph() {
            graphctx.lineWidth = 0.5;
            for (const name in nodes) get_node_coordinates(name);
            for (const name in nodes) {
                const node = nodes[name];
                for (const index in node.neighbors) {
                    const neighbor_name = node.neighbors[index];
                    const neighbor = nodes[neighbor_name];
                    if(typeof neighbor == "undefined") continue;
                    if(name < neighbor_name) continue;
                    graphctx.strokeStyle = node.representation.length%2==0?YELLOW:RED;
                    graphctx.beginPath();
                    graphctx.moveTo(node.screen_x, node.screen_y);
                    graphctx.lineTo(neighbor.screen_x, neighbor.screen_y);
                    graphctx.stroke();
                }
                if(config.blurbs.select == 1 && c4[node.representation] != undefined && (c4[node.representation].name.length > 0 || c4[node.representation].blurb.length > 0)){
                    graphctx.globalAlpha = 1;
                    graphctx.strokeStyle = `white`;
                    graphctx.beginPath();
                    graphctx.arc(node.screen_x, node.screen_y, 5, 0, 2*Math.PI);
                    graphctx.stroke();
                }
            }
            graphctx.globalAlpha = 1;
            graphctx.strokeStyle = `white`;
            graphctx.lineWidth = 2;
            graphctx.beginPath();
            graphctx.arc(nodes[hash].screen_x, nodes[hash].screen_y, 10, 0, 2*Math.PI);
            graphctx.stroke();

        }

        function render_blurb(){
            graphctx.globalAlpha = 1;
            var y = h - 180;
            graphctx.fillStyle = "white";
            graphctx.font = "16px Arial";
            graphctx.fillText("Controls", 20, y+=16)
            graphctx.fillText("[R] to reset", 20, y+=16)
            graphctx.fillText("[A/D] rotate graph", 20, y+=16)
            graphctx.fillText("[arrows] to pan", 20, y+=16)
            graphctx.fillText("[click] on a node!", 20, y+=16)
        }

        function render_data(){
            if(c4[nodes[hash].representation] == undefined) return;
            graphctx.globalAlpha = 1
            var y = boardcanvas.height+30;
            graphctx.fillStyle = "white";
            graphctx.font = "16px Arial";
            graphctx.fillText("Opening Name:", 20, y+=16)
            graphctx.font = "30px Arial";
            graphctx.fillText(c4[nodes[hash].representation].name, 20, y+=30)
            graphctx.font = "16px Arial";
            graphctx.fillText("Description:", 20, y+=30)
            graphctx.fillText(c4[nodes[hash].representation].blurb, 20, y+=30)
        }

        function render () {
            graphctx.globalAlpha = 1;
            graphctx.fillStyle = `Black`;
            graphctx.fillRect(0, 0, w, h);
            
            render_graph();
            render_blurb();
            //render_data();
        }

        function get_node_coordinates (hash) {
            var node = nodes[hash];
            node.screen_x = (node.x*Math.cos(alpha)+node.z*Math.sin(alpha) - ox) / zoom + w / 2;
            node.screen_y = (node.y - oy) / zoom + h / 2;
        }

        function get_closest_node_to (coords) {
            var min_dist = 100000000;
            var best_node = "";
            for (const name in nodes) {
                const node = nodes[name];
                var d = Math.hypot(node.screen_x-coords.x, node.screen_y-coords.y);
                if (d < min_dist) {
                    min_dist = d;
                    best_node = name;
                }
            }
            console.log(min_dist)
            if(min_dist > 30) return hash;
            return best_node;
        }

        function color_wheel(angle, brightness=1){
            angle*=0.025;
            var r = normsin(angle)*brightness;
            var g = normsin(angle+Math.PI*2.0/3)*brightness;
            var b = normsin(angle+Math.PI*4.0/3)*brightness;
            return "rgb("+r+","+g+","+b+")";
        }

        function normsin(angle){
            return Math.floor(128.0*(Math.sin(angle)+1));
        }

        window.addEventListener(`wheel`,
            (event) => {
                zoom *= Math.pow(1.2, Math.sign(event.deltaY));
                render();
            }
        );
        graphcanvas.addEventListener(`mousedown`, function(e){
            graphbutton = true;
            var rect = graphcanvas.getBoundingClientRect();
            var screen_coords = {
                x: e.clientX - rect.left,
                y: e.clientY - rect.top
            };
            hash = get_closest_node_to(screen_coords);
            board_string = nodes[hash].representation;
            render();
            render_board();
        }, false);
        graphcanvas.addEventListener(`mouseup`, function(e){
            graphbutton = false;
        }, false);
        graphcanvas.addEventListener(`mousemove`, function(e){
            if(!graphbutton) return;
            var rect = graphcanvas.getBoundingClientRect();
            var screen_coords = {
                x: e.clientX - rect.left,
                y: e.clientY - rect.top
            };
        }, false);

        window.addEventListener(`keydown`, key, false);

        function key (e) {
            const c = e.keyCode;
            if (c == 37) ox -= zoom * 100;
            if (c == 38) oy -= zoom * 100;
            if (c == 39) ox += zoom * 100;
            if (c == 40) oy += zoom * 100;
            if (c == 65) alpha -= .04;
            if (c == 68) alpha += .04;
            if (c == 66) config.blurbs.select = (config.blurbs.select+1)%config.blurbs.options.length;
            if (c == 82) board_string = save_start_board;
            render();
        }











        var square_sz = 40;

        const boardcanvas = document.getElementById(`board`);
        boardcanvas.width = (parseInt(board_w)+1)*square_sz;
        boardcanvas.height = (parseInt(board_h)+1)*square_sz;
        const boardctx = boardcanvas.getContext(`2d`);

        let boardbutton = false;
        let board_click_start = {x:0,y:0};
        let diffcoords = {x:0,y:0};
        let board_click_square = ';';

        var EMPTY_SPACE = '.';

        render();

        function drawStone(x, y, col) {
         var px = (x+.5)*40;
         var py = (5-y+.5)*40;

         boardctx.fillStyle = col;
         boardctx.beginPath();
         boardctx.arc(px, py, 20, 0, 2*Math.PI, false);
         boardctx.fill();
        }


        function render_board () {
            boardcanvas.width = boardcanvas.width;
        var grid = [];
        for(let y=0; y<6; y++) {
         grid[y] = [];
         for(let x=0; x<7; x++) {
          grid[y][x] = EMPTY;
         }
        }
        console.log(board_string)
        for (var i = 0; i < board_string.length; i++){
         var x = String.fromCharCode(board_string.charCodeAt(i))-1;

         // place the piece
         var col = (i%2==0)?RED:YELLOW;
         for (var y=0; y<6; y++) {
          if(grid[y][x] === EMPTY){
           grid[y][x] = col;
              break;
          }
         }

        }
         for(var x = 0; x < 7; x++)
          for(var y = 0; y < 6; y++)
           drawStone(x, y, grid[y][x]);
        }
        render_board();


    } catch (error) {
        console.error('Error loading or parsing data:', error);
    }
});