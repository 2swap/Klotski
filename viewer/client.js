var nodes_to_use = false;
let nodes = false;
var hash = 0;
var parsedData = 0;

const boardcanvas = document.getElementById(`board`);
const boardctx = boardcanvas.getContext(`2d`);

let boardbutton = false;
let board_click_start = {x:0,y:0};
let diffcoords = {x:0,y:0};
let board_click_square = ';';

var config = {};

var dataset = "default";
// Split the URL to get the query string
var queryString = window.location.href.split('?')[1];
if (queryString) {
  // Create an object to store the parameters
  var params = {};
  // Iterate through the key-value pairs and store them in the object
  queryString.split('&').forEach(function (param) {
    var keyValue = param.split('=');
    params[keyValue[0]] = keyValue[1];
  });
  // Get the value of the "dataset" parameter or use the default value if it's not provided
  dataset = params['dataset'];  
  // Use the "dataset" value as needed
}

console.log("dataset:", dataset);

on_board_change = function(){};

function loadJS(FILE_URL) {
    return new Promise((resolve, reject) => {
        let scriptEle = document.createElement("script");

        scriptEle.setAttribute("src", FILE_URL);
        scriptEle.setAttribute("type", "text/javascript");
        scriptEle.setAttribute("async", false);

        // success event 
        scriptEle.addEventListener("load", () => {
            console.log("File loaded");
            resolve(); // Resolve the Promise when the script is loaded
        });

        // error event
        scriptEle.addEventListener("error", (ev) => {
            console.log("Error on loading file", ev);
            reject(ev); // Reject the Promise if there's an error
        });

        document.body.appendChild(scriptEle);
    });
}

// Check if the URL contains "localhost", i.e. in a development environment
cachebust = window.location.href.indexOf("localhost") !== -1 ? "?cachebust=" + new Date().getTime() : "";

$(document).ready(async function() {
    try {

        const datasets_list_response = await fetch('datasets.json' + cachebust);
        if (!datasets_list_response.ok) {
            throw new Error('Failed to fetch datasets list');
        }
        var datasets = await datasets_list_response.json();
        datasets = datasets.datasets;
        if(dataset == 'default') dataset = datasets[0];
        
        // Find the index of the current dataset in the array
        var currentIndex = datasets.indexOf(dataset);
        // Calculate the index of the next dataset
        var nextIndex = (currentIndex + 1) % datasets.length;
        // Get the current URL and split it into parts
        var currentURL = window.location.href;
        var urlParts = currentURL.split('?');
        // Construct the URL for the next dataset
        var nextDataset = datasets[nextIndex];
        var nextURL = urlParts[0] + '?dataset=' + nextDataset;

        const specific_dataset_response = await fetch('data/' + dataset + '.json' + cachebust);
        if (!specific_dataset_response.ok) {
            throw new Error('Failed to fetch specific dataset');
        }
        parsedData = await specific_dataset_response.json();
        nodes_to_use = parsedData.nodes_to_use;

        await loadJS(parsedData.game_name + ".js" + cachebust);

        const graphcanvas = document.getElementById(`graph`);
        const w = graphcanvas.width = window.innerWidth;
        const h = graphcanvas.height = window.innerHeight;
        const graphctx = graphcanvas.getContext(`2d`);

        let tick = 0;
        let ox = 0; let oy = 100; let zoom = 1;
        let alpha = 0.8, beta=0;

        nodes = {};

        function reset_hash(){
            hash = parsedData.root_node_hash;
        }

        reset_hash();

        resizePointCloud();

        function resizePointCloud() {
            var sqrtwh = Math.sqrt(w * h);
            var min_x = Number.POSITIVE_INFINITY;
            var max_x = Number.NEGATIVE_INFINITY;
            var min_y = Number.POSITIVE_INFINITY;
            var max_y = Number.NEGATIVE_INFINITY;
            var min_z = Number.POSITIVE_INFINITY;
            var max_z = Number.NEGATIVE_INFINITY;

            // Compute the min and max values for x, y, and z
            for (const name in nodes_to_use) {
                const node = nodes_to_use[name];
                min_x = Math.min(min_x, node.x);
                max_x = Math.max(max_x, node.x);
                min_y = Math.min(min_y, node.y);
                max_y = Math.max(max_y, node.y);
                min_z = Math.min(min_z, node.z);
                max_z = Math.max(max_z, node.z);
            }

            // Calculate the center of the point cloud
            var center_x = (min_x + max_x) / 2;
            var center_y = (min_y + max_y) / 2;
            var center_z = (min_z + max_z) / 2;

            // Calculate the scale factor
            var max_dimension = Math.max(max_x - min_x, max_y - min_y, max_z - min_z);
            var scale_factor = (sqrtwh / 2) / max_dimension;

            // Apply the transformation to each node
            for (const name in nodes_to_use) {
                var node = nodes_to_use[name];
                node.x = (node.x - center_x) * scale_factor;
                node.y = (node.y - center_y) * scale_factor;
                node.z = (node.z - center_z) * scale_factor;
                nodes[name] = node;
                delete nodes_to_use[name];
            }
        }

        function render_blurb(){
            graphctx.textAlign = 'left';
            graphctx.globalAlpha = 1;
            var y = h - 280;
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
            graphctx.fillText("", 20, y+=16)
            for(s in game_blurb){
                graphctx.fillText(game_blurb[s], 20, y+=16)
            }
        }

        function render_graph() {
            graphctx.globalAlpha = 1;
            graphctx.lineWidth = 0.5;
            for (const name in nodes) get_node_coordinates(name);
            for (const name in nodes) {
                const node = nodes[name];
                for (const neighbor_idx in node.neighbors) {
                    const neighbor_name = node.neighbors[neighbor_idx];
                    const neighbor = nodes[neighbor_name];
                    if(typeof neighbor == "undefined") continue;
                    if(name < neighbor_name && (neighbor.neighbors != null && name in neighbor.neighbors)) continue;
                    graphctx.strokeStyle = get_color(name, neighbor_name);
                    graphctx.beginPath();
                    graphctx.moveTo(node.screen_x, node.screen_y);
                    graphctx.lineTo(neighbor.screen_x, neighbor.screen_y);
                    graphctx.stroke();
                }
                if(config.solutions.select == 1 && node.solution_dist == 0){
                    graphctx.strokeStyle = `white`;
                    graphctx.beginPath();
                    graphctx.arc(node.screen_x, node.screen_y, 5, 0, 2*Math.PI);
                    graphctx.stroke();
                }
            }
            graphctx.strokeStyle = `white`;
            graphctx.lineWidth = 2;
            graphctx.beginPath();
            graphctx.arc(nodes[hash].screen_x, nodes[hash].screen_y, 10, 0, 2*Math.PI);
            graphctx.stroke();

            if(config.path.select == 1){
                var curr_node = nodes[hash];
                while(curr_node.solution_dist && curr_node.solution_dist != 0){
                    for(i in curr_node.neighbors){
                        var neighbor = nodes[curr_node.neighbors[i]];
                        if(neighbor.solution_dist < curr_node.solution_dist){

                            graphctx.beginPath();
                            graphctx.moveTo(curr_node.screen_x, curr_node.screen_y);
                            graphctx.lineTo(neighbor.screen_x, neighbor.screen_y);
                            graphctx.stroke();

                            curr_node = neighbor;
                            break;
                        }
                    }
                }
            }

        }

        function uppercase_it(str){
            return str.charAt(0).toUpperCase() + str.slice(1);
        }

        function render() {
            graphctx.globalAlpha = 1;
            graphctx.fillStyle = `Black`;
            graphctx.fillRect(0, 0, w, h);
            graphctx.textBaseline = 'bottom';

            render_graph();
            render_blurb();
            // render_histogram();

            // Add text to the bottom right corner
            graphctx.fillStyle = 'White';
            graphctx.font = '32px Arial';
            graphctx.textAlign = 'right';
            graphctx.fillText('Press N to view another graph!', w - 10, h - 10);
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
            if(min_dist > 150) return hash;
            return best_node;
        }

        window.addEventListener(`wheel`,
            (event) => {
                zoom *= Math.pow(1.2, Math.sign(event.deltaY));
                render();
            }
        );
        graphcanvas.addEventListener(`mousedown`, function(e){
            var rect = graphcanvas.getBoundingClientRect();
            var screen_coords = {
                x: e.clientX - rect.left,
                y: e.clientY - rect.top
            };
            hash = get_closest_node_to(screen_coords);
            console.log(hash);
            on_board_change();
        }, false);

        window.addEventListener(`keydown`, key, false);

        function key (e) {
            const c = e.keyCode;
            const ch = String.fromCharCode(c);
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
            if (ch == 'N') window.location.href = nextURL;
            on_board_change();
        }

        on_board_change = function(){
            render();
            render_board();
        }

        on_board_change();

    } catch (error) {
        console.error('Error loading or parsing data:', error);
    }
});