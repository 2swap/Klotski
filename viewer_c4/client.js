const graphcanvas = document.getElementById(`graph`);
const w = graphcanvas.width = window.innerWidth;
const h = graphcanvas.height = window.innerHeight;
const graphctx = graphcanvas.getContext(`2d`);

let tick = 0;
let ox = 0; let oy = 100; let zoom = 1;
let alpha = 0, beta=0;
let graphbutton = false;

var config = {
    blurbs: {options:["Invisible"/*, "Visible"*/], select:0, key:'B'},
    solutions: {options:["Invisible", "Visible"], select:0, key:'Q'},
    mode: {options:["Explore", "Play"], select:0, key:'M'},
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
        var histogram_non_solutions = parsedData.histogram_non_solutions;
        var histogram_solutions = parsedData.histogram_solutions;
        var nodes_to_use = parsedData.nodes_to_use;
        var rushhour = parsedData.rushhour;

        let nodes = {};
        var boardctx = false;

        increment_max();
        reset_hash();

        function increment_max(){
            var max_x = 0;
            for (const name in nodes_to_use){
                max_x = Math.max(nodes_to_use[name].x, max_x);
            }
            for (const name in nodes_to_use){
                node = nodes_to_use[name];
                nodes[name] = node;
                node.x*=w*.2/max_x;
                node.y*=w*.2/max_x;
                node.z*=w*.2/max_x;
                delete nodes_to_use[name];
            }
        }
        function reset_hash(){
            for (const name in nodes){
                if(nodes[name].dist == 0) hash = name;
            }
            render();
            if(boardctx)render_board();
        }

        var dEMPTY      = "#026";
        var dRED        = "#900";
        var dYELLOW     = "#760";
        var EMPTY       = "#049";
        var RED         = "#f00";
        var YELLOW      = "#ff0";

        function render_graph() {
            for (const name in nodes) get_node_coordinates(name);
            for (const name in nodes) {
                graphctx.lineWidth = 0.5;
                const node = nodes[name];
                for (const index in node.neighbors) {
                    const neighbor_name = node.neighbors[index];
                    const neighbor = nodes[neighbor_name];
                    if(typeof neighbor == "undefined") continue;
                    if(name < neighbor_name) continue;
                    graphctx.strokeStyle = Math.min(node.representation.length, neighbor.representation.length)%2==1?YELLOW:RED;
                    if(node.highlight && neighbor.highlight) {graphctx.strokeStyle = "lime"; graphctx.lineWidth = 1;}
                    graphctx.beginPath();
                    graphctx.moveTo(node.screen_x, node.screen_y);
                    graphctx.lineTo(neighbor.screen_x, neighbor.screen_y);
                    graphctx.stroke();
                }
                if((config.solutions.select == 1 && node.solution_dist == 0) || (config.blurbs.select == 1 && c4[node.representation] != undefined && (c4[node.representation].name.length > 0 || c4[node.representation].blurb.length > 0))){
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

        function uppercase_it(str){
            return str.charAt(0).toUpperCase() + str.slice(1);
        }

        function render_blurb(){
            graphctx.globalAlpha = 1;
            var y = h - 180;
            graphctx.fillStyle = "white";
            graphctx.font = "16px Arial";
            graphctx.fillText("Controls", 20, y+=16)
            graphctx.fillText("[R] to reset", 20, y+=16)
            graphctx.fillText("[WASD] rotate graph", 20, y+=16)
            for(x in config){
                graphctx.fillText("["+config[x].key+"] "+uppercase_it(x)+": " + config[x].options[config[x].select], 20, y+=16)
            }
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
            var rotatedX = node.x * Math.cos(alpha) + node.z * Math.sin(alpha);
            var rotatedZ = -node.x * Math.sin(alpha) + node.z * Math.cos(alpha);
            var rotatedY = rotatedZ * Math.sin(beta) + node.y * Math.cos(beta);

            node.screen_x = (rotatedX - ox) / zoom + w / 2;
            node.screen_y = (rotatedY - oy) / zoom + h / 2;
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
            const ch = String.fromCharCode(c);
            console.log(ch);
            if (c == 37) ox -= zoom * 100;
            if (c == 38) oy -= zoom * 100;
            if (c == 39) ox += zoom * 100;
            if (c == 40) oy += zoom * 100;
            if (ch == 'A') alpha -= .04;
            if (ch == 'D') alpha += .04;
            if (ch == 'S') beta -= .04;
            if (ch == 'W') beta += .04;
            for(x in config){
                if (ch == config[x].key) config[x].select = (config[x].select+1)%config[x].options.length;
            }
            if (ch == 'R') reset_hash();
            makeMoveAsRed(nodes[hash].representation);
            render();
        }











        var square_sz = 40;

        const boardcanvas = document.getElementById(`board`);
        boardcanvas.width = (parseInt(board_w))*square_sz;
        boardcanvas.height = (parseInt(board_h))*square_sz;
        boardctx = boardcanvas.getContext(`2d`);

        let boardbutton = false;
        let board_click_start = {x:0,y:0};
        let diffcoords = {x:0,y:0};
        let board_click_square = ';';

        var EMPTY_SPACE = '.';

        render();

        function drawStone(x, y, col) {
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

          // Call a function to handle the player's move in this column
          makeMove(column + 1);
        }

        function makeMoveAsRed(newBoardString){
          if(config.mode.select == 1 && newBoardString.length%2 == 0)
            setTimeout(function () {
              for (let column = 1; column <= 7; column++) {
                if (makeMove(column)) {
                  break;
                }
              }
            }, 400);
        }

        function makeMove(column) {
            console.log(column);
            const newBoardString = nodes[hash].representation + column;

              // Update the board representation and hash
              for (const nodeHash in nodes) {
                  if(nodes[nodeHash].representation == newBoardString){
                      hash = nodeHash;

                      // Render the updated board
                      makeMoveAsRed(newBoardString);
                      render_board();
                      render();
                    return true;
                  }

              }
              return false;
        }



        function render_board () {
            boardcanvas.width = boardcanvas.width;
            boardctx.font = "24px Arial";
            boardctx.textAlign = "center";
        var grid = [];
        for(let y=0; y<6; y++) {
         grid[y] = [];
         for(let x=0; x<7; x++) {
          grid[y][x] = dEMPTY;
         }
        }
        var board_string = nodes[hash].representation;
        console.log(board_string)
        for (var i = 0; i < board_string.length; i++){
         var x = String.fromCharCode(board_string.charCodeAt(i))-1;

         // place the piece
         var col = (i%2==0)?dRED:dYELLOW;
         for (var y=0; y<6; y++) {
          if(grid[y][x] === dEMPTY){
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