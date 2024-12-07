config = {
};
const game_blurb = [
    "This is a complete weak solution of connect-4 with <nodes> nodes.",
    "It is (close to) the minimum amount of information which you need to",
    "'memorize' in order to always win, as player 1 (red).",
    "Terminal nodes contain 'steady state diagrams', a language which expresses",
    "low-information weak solutions of sufficiently developed positions."
]
var EMPTY       = "#049";
var RED         = "#f00";
var YELLOW      = "#ff0";










function get_color(name, neighbor_name){
    return Math.min(nodes[name].rep.length, nodes[neighbor_name].rep.length)%2==1?YELLOW:RED
}

let board_arr = 0;

var square_sz = 40;
var extraMoves = "";

function get_hash() {
    let a = 1;
    let semihash = 0;

    for (let i = 0; i < parsedData.board_h; i++) {
        for (let j = 0; j < parsedData.board_w; j++) {
            semihash += board_arr[parsedData.board_h-1-i][j] * a;
            a *= 1.021813947;
        }
    }

    var closedist = 0.00000001;
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
    boardcanvas.width = parseInt(parsedData.board_w) * square_sz;
    boardcanvas.height = parseInt(parsedData.board_h) * square_sz + 54;
    boardctx.font = "24px Arial";
    boardctx.textAlign = "center";
    board_arr = [];

    // Initialize the board array
    for (let y = 0; y < 6; y++) {
        board_arr[y] = [];
        for (let x = 0; x < 7; x++) {
            board_arr[y][x] = 0;
        }
    }

    var board_string = repstr();
    for (var i = 0; i < board_string.length; i++) {
        var x = String.fromCharCode(board_string.charCodeAt(i)) - 1;

        // Place the piece
        for (var y = 0; y < 6; y++) {
            if (board_arr[y][x] === 0) {
                board_arr[y][x] = i % 2 + 1;
                break;
            }
        }
    }

    // Detect winning lines
    const winningLine = checkForWin(board_arr);

    // Draw the board
    for (var x = 0; x < 7; x++) {
        for (var y = 0; y < 6; y++) {
            drawStone(x, y, board_arr[y][x], winningLine);
        }
    }

    boardctx.font = "15px Arial";
    boardctx.textAlign = "left";
    boardctx.fillStyle = "white";
    if(winningLine)
        boardctx.fillText("Press 'r' to reset!", 8, 16+square_sz * parsedData.board_h);
    else if(extraMoves == "")
        boardctx.fillText("Click to play against the weak solution!", 8, 16+square_sz * parsedData.board_h);
    else {
        var dy = 0;
        boardctx.fillText("This is a Steady State Diagram,"  , 8, (dy+=16)+square_sz * parsedData.board_h);
        boardctx.fillText("which instructs the agent to play", 8, (dy+=16)+square_sz * parsedData.board_h);
        boardctx.fillText("perfectly from here on."          , 8, (dy+=16)+square_sz * parsedData.board_h);
    }
}

// Helper function to draw a stone
function drawStone(x, y, col, winningLine) {
    col = ["#026", "#900", "#760"][col];
    const px = (x + 0.5) * square_sz;
    const py = (5 - y + 0.5) * square_sz;

    boardctx.fillStyle = col;
    boardctx.beginPath();
    boardctx.arc(px, py, 18, 0, 2 * Math.PI, false);
    boardctx.fill();

    // Highlight winning stones
    if (winningLine && winningLine.some(([wy, wx]) => wx === x && wy === y)) {
        boardctx.fillStyle = "gold"; // Highlight color
        boardctx.beginPath();
        boardctx.arc(px, py, 10, 0, 2 * Math.PI, false);
        boardctx.fill();
    }

    boardctx.fillStyle = "white";

    // Draw steady state markers if needed
    const ss = String.fromCharCode(nodes[hash].data.ss[5 - y][x]);
    if (ss !== '1' && ss !== '2') {
        boardctx.fillText(ss, px, py + 9);
    }
}

// Helper function to check for a win
function checkForWin(board) {
    const directions = [
        { dx: 1, dy: 0 }, // Horizontal
        { dx: 0, dy: 1 }, // Vertical
        { dx: 1, dy: 1 }, // Diagonal down-right
        { dx: 1, dy: -1 } // Diagonal up-right
    ];

    for (let y = 0; y < 6; y++) {
        for (let x = 0; x < 7; x++) {
            const player = board[y][x];
            if (player === 0) continue; // Skip empty cells

            for (let { dx, dy } of directions) {
                const line = [[y, x]];

                for (let step = 1; step < 4; step++) {
                    const nx = x + dx * step;
                    const ny = y + dy * step;

                    if (nx < 0 || nx >= 7 || ny < 0 || ny >= 6 || board[ny][nx] !== player) {
                        break;
                    }

                    line.push([ny, nx]);
                }

                if (line.length === 4) {
                    return line; // Return the winning line
                }
            }
        }
    }

    return null; // No winning line found
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
    makeMoveAsYellow(column + 1);
}

function makeMoveAsRed(){
    if(repstr().length%2 == 1) return;
    var thishash = get_hash();
    if(nodes[thishash] && nodes[thishash].neighbors){
        for(var neighbor_id in nodes[thishash].neighbors){
            var neighbor_hash = nodes[thishash].neighbors[neighbor_id];
            var neighbor = nodes[neighbor_hash];
            if(neighbor.rep.length > nodes[thishash].rep.length){
                hash = neighbor_hash;
                extraMoves = "";
                on_board_change();
                return;
            }
        }
    }

    else {
        console.log("Attempting steadystate");
        extraMoves += querySteadyState(board_arr, nodes[hash].data.ss);
        on_board_change();
    }
}

function querySteadyState(boardArr, steadyState) {
    const ROWS = 6;
    const COLUMNS = 7;

    // Helper to get the state of a column
    function getColumnState(x) {
        for (let y = 0; y < ROWS; y++) {
            if (boardArr[y][x] === 0) return y; // Find the first empty spot in column x
        }
        return -1; // Column is full
    }

    // Check if placing a piece in column x wins the game for the given player
    function checkWin(board, x, player) {
        const y = getColumnState(x);
        if (y === -1) return false; // Column is full
        board[y][x] = player; // Temporarily place the piece
        const isWin = checkFourInARow(board, x, y, player); // Check win condition
        board[y][x] = 0; // Undo the temporary placement
        return isWin;
    }

    // Check if there are four in a row
    function checkFourInARow(board, x, y, player) {
        // Check horizontal, vertical, and two diagonals
        const directions = [
            { dx: 1, dy: 0 }, { dx: 0, dy: 1 },
            { dx: 1, dy: 1 }, { dx: 1, dy: -1 }
        ];
        for (let { dx, dy } of directions) {
            let count = 1;
            for (let sign = -1; sign <= 1; sign += 2) {
                for (let step = 1; step < 4; step++) {
                    const nx = x + dx * step * sign;
                    const ny = y + dy * step * sign;
                    if (nx < 0 || ny < 0 || nx >= COLUMNS || ny >= ROWS) break;
                    if (board[ny][nx] !== player) break;
                    count++;
                }
            }
            if (count >= 4) return true;
        }
        return false;
    }

    // Decode steady state character to priority
    function decodePriority(c) {
        switch (String.fromCharCode(c)) {
            case '@': return 'miai';
            case ' ': case '.': return 'claimeven';
            case '|': return 'claimodd';
            case '+': return 'plus';
            case '=': return 'equal';
            case '-': return 'minus';
            case '1': return 'red';
            case '2': return 'yellow';
            case '!': return 'urgent';
            default: throw new Error(`Invalid character in steadyState: ${String.fromCharCode(c)}`);
        }
    }

    // Identify instant win and blocking moves
    for (let x = 0; x < COLUMNS; x++) {
        if (getColumnState(x) !== -1) {
            // Check for an instant win
            if (checkWin(boardArr, x, 1)) return x + 1; // Player 1 wins
            // Check for a blocking move
            if (checkWin(boardArr, x, 2)) return x + 1; // Block Player 2's win
        }
    }

    // Priority order
    const priorities = ['urgent', 'miai', 'claimeven', 'claimodd', 'plus', 'equal', 'minus'];

    for (let priority of priorities) {
        let validMoves = [];
        console.log("CURRENT: " + priority);
        for (let x = 0; x < COLUMNS; x++) {
            y = getColumnState(x);
            if(y == -1) continue;
            y = 5-y;
            const ch = steadyState[y][x];
            console.log(decodePriority(ch) + " y: " + y);
            if (decodePriority(ch) === priority) {
                // Handle special cases
                if (priority === 'miai') {
                    validMoves.push(x);
                    if (validMoves.length > 1) break; // Ignore if more than one miai
                } else if (priority === 'claimeven') {
                    if (y % 2 === 0) return x + 1; // Only valid for even rows
                } else if (priority === 'claimodd') {
                    if (y % 2 === 1) return x + 1; // Only valid for odd rows
                } else {
                    return x + 1; // Return move for other priorities
                }
            }
        }
        // If only one valid miai, return it
        if (priority === 'miai' && validMoves.length === 1) return validMoves[0] + 1;
    }

    // No valid move found
    return -4;
}

setInterval(makeMoveAsRed, 300);

function on_click_node(){ extraMoves = ""; }

function repstr(){
    return nodes[hash].rep + extraMoves;
}

function makeMoveAsYellow(column) {
    if(checkForWin(board_arr) != null) return;
    if(repstr().length%2 == 0) return;
    let x = column-1;

    let who = nodes[hash].rep.length%2 + 1

    // place the piece
    for (var y=0; y<6; y++) {
        if(board_arr[y][x] === 0){
            board_arr[y][x] = who;
            break;
        }
        if(y == 5) return false;
    }
    newhash = get_hash();
    if (newhash != -1) {
        hash = newhash;
        extraMoves = "";
    }
    else extraMoves += column;
    on_board_change();
    makeMoveAsRed();
    return newhash != -1;
}
