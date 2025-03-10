const canvas = document.getElementsByTagName("canvas")[0]
const modal = document.getElementsByClassName("modal")
const pieces = document.getElementsByClassName("piece")
const result = document.getElementsByClassName("result")[0]
const ctx = canvas.getContext("2d")
const game = {
    socket: new WebSocket("ws://localhost:3000/"),
    assets: {},
    pieces: [],
    piece: { square: {} },
    pieceMoves: [],
    moves: [],
    move: {},
    state: 'none',
    turn: 'white',
}

function render() {
    canvas.width = canvas.getBoundingClientRect().width
    canvas.height = canvas.getBoundingClientRect().height
    const squareWidth = canvas.height / 8

    game.pieces[12].check = isCheck("white")
    game.pieces[28].check = isCheck("black")

    for (let i = 0; i < 8; i++) {
        for (let j = 0; j < 8; j++) {
            const moves = [...game.pieceMoves, game.piece]
            const isMove = moves.find(move => move.square.x == i && move.square.y == j)
            const piece = game.pieces.find(piece => piece.square.x == i && piece.square.y == j)
            const isCheck = piece ? piece.check : false
            ctx.fillStyle = isCheck ? "red" : isMove ? "lime" : "black"
            if ((i + j) % 2 != 0 || isMove || isCheck) {
                ctx.fillRect(squareWidth * i, squareWidth * (7 - j), squareWidth, squareWidth)
            }
        }
    }

    for (const piece of game.pieces) {
        if (piece.alive) {
            ctx.drawImage(
                game.assets[piece.img],
                piece.square.x * squareWidth,
                (7 - piece.square.y) * squareWidth,
                squareWidth,
                squareWidth
            )
        }
    }
}

function load() {
    const fileNames = [
        'white-pawn', 'white-knight', 'white-bishop', 'white-rook', 'white-queen', 'white-king',
        'black-pawn', 'black-knight', 'black-bishop', 'black-rook', 'black-queen', 'black-king',
    ]
    return Promise.all(fileNames.map(name =>
        loadImage(`./assets/${name}.png`)
            .then(file => game.assets[name] = file)
    ))
}

function loadImage(url) {
    return new Promise((resolve, reject) => {
        const img = new Image();
        img.src = url;
        img.onerror = () => reject(img);
        img.onload = () => resolve(img);
    });
}

function setup() {
    game.pieces.push(
        { id: 0, square: { x: 0, y: 1 }, img: 'white-pawn' },
        { id: 1, square: { x: 1, y: 1 }, img: 'white-pawn' },
        { id: 2, square: { x: 2, y: 1 }, img: 'white-pawn' },
        { id: 3, square: { x: 3, y: 1 }, img: 'white-pawn' },
        { id: 4, square: { x: 4, y: 1 }, img: 'white-pawn' },
        { id: 5, square: { x: 5, y: 1 }, img: 'white-pawn' },
        { id: 6, square: { x: 6, y: 1 }, img: 'white-pawn' },
        { id: 7, square: { x: 7, y: 1 }, img: 'white-pawn' },
        { id: 8, square: { x: 0, y: 0 }, img: 'white-rook' },
        { id: 9, square: { x: 1, y: 0 }, img: 'white-knight' },
        { id: 10, square: { x: 2, y: 0 }, img: 'white-bishop' },
        { id: 11, square: { x: 3, y: 0 }, img: 'white-queen' },
        { id: 12, square: { x: 4, y: 0 }, img: 'white-king' },
        { id: 13, square: { x: 5, y: 0 }, img: 'white-bishop' },
        { id: 14, square: { x: 6, y: 0 }, img: 'white-knight' },
        { id: 15, square: { x: 7, y: 0 }, img: 'white-rook' },
        { id: 16, square: { x: 0, y: 6 }, img: 'black-pawn' },
        { id: 17, square: { x: 1, y: 6 }, img: 'black-pawn' },
        { id: 18, square: { x: 2, y: 6 }, img: 'black-pawn' },
        { id: 19, square: { x: 3, y: 6 }, img: 'black-pawn' },
        { id: 20, square: { x: 4, y: 6 }, img: 'black-pawn' },
        { id: 21, square: { x: 5, y: 6 }, img: 'black-pawn' },
        { id: 22, square: { x: 6, y: 6 }, img: 'black-pawn' },
        { id: 23, square: { x: 7, y: 6 }, img: 'black-pawn' },
        { id: 24, square: { x: 0, y: 7 }, img: 'black-rook' },
        { id: 25, square: { x: 1, y: 7 }, img: 'black-knight' },
        { id: 26, square: { x: 2, y: 7 }, img: 'black-bishop' },
        { id: 27, square: { x: 3, y: 7 }, img: 'black-queen' },
        { id: 28, square: { x: 4, y: 7 }, img: 'black-king' },
        { id: 29, square: { x: 5, y: 7 }, img: 'black-bishop' },
        { id: 30, square: { x: 6, y: 7 }, img: 'black-knight' },
        { id: 31, square: { x: 7, y: 7 }, img: 'black-rook' }
    )
    game.pieces.forEach(piece => piece.alive = true)
    load().then(() => render())
}

function update(move) {
    const targetY = move.square.y + (move.type == 2 ? game.turn == "white" ? -1 : 1 : 0)
    for (const piece of game.pieces) {
        if (piece.square.x == move.square.x &&
            piece.square.y == targetY) {
            piece.alive = false
        }
    }
    game.piece.square = move.square
    game.turn = game.turn == "white" ? "black" : "white"
    game.pieceMoves = []
    game.piece = { square: {} }
    game.move = {}
}

function onclick(e) {
    const squareWidth = canvas.height / 8
    const offsetX = canvas.getBoundingClientRect().left
    const offsetY = canvas.getBoundingClientRect().top
    const squareX = Math.floor((e.clientX - offsetX) / squareWidth)
    const squareY = 7 - Math.floor((e.clientY - offsetY) / squareWidth)

    for (const move of game.pieceMoves) {
        if (move.square.x == squareX && move.square.y == squareY && move.id == game.piece.id) {
            if (move.type == 4) {
                const moves = game.pieceMoves.filter(move =>
                    move.type == 4 && move.square.x == squareX
                    && move.square.y == squareY)
                for (let i = 0; i < 4; i++) {
                    pieces[i].dataset.move = moves[i].index
                }
                modal[0].style.display = "block"
                return game.move = move
            } else {
                game.socket.send(new Uint8Array([1, move.index]))
                update(move)
                return render()
            }
        }
    }
    for (const piece of game.pieces) {
        if (piece.square.x == squareX && piece.square.y == squareY &&
            piece.img.startsWith(game.turn) && piece.alive
        ) {
            game.pieceMoves = game.moves.filter(move => move.id == piece.id)
            game.piece = piece
            return render()
        }
    }
    game.pieceMoves = []
    game.piece = { square: {} }
    return render()
}

async function onmessage(message) {
    const buffer = new Uint8Array(await message.data.arrayBuffer());
    let move;
    const isFirst = game.moves.length == 0
    game.moves = []
    if (buffer[0] == 2) {
        for (let i = 0; i < (buffer.length - 1) / 4; i++) {
            game.moves.push({
                index: i - 1,
                id: buffer[i * 4 + 1],
                type: buffer[i * 4 + 2],
                square: { x: buffer[i * 4 + 3], y: buffer[i * 4 + 4] },
            })
        }
        if (!isFirst) {
            move = game.moves.shift();
            game.piece = game.pieces[move.id]
            update(move)
            render()
        }
    }
    if (game.moves.length == 0) {
        result.childNodes[3].innerText = `${game.turn == "white" ? "Black" : "White"} wins!`
        result.style.display = "block";
        game.socket.close()
    }
    render()
}

function onopen() {
    game.socket.send(new Uint8Array([0, 1]))
}

function onpromote(move) {
    game.socket.send(new Uint8Array([1, move.dataset.move]))
    const piece = game.pieces.find(piece => piece.id == game.move.id)
    piece.img = new URL(move.src).pathname.slice(8, -4)
    update(game.move)
    render()
    modal[0].style.display = "none"
}

for (const piece in pieces) {
    pieces[piece].onclick = () => onpromote(pieces[piece])
}

function isCheckInner(piece, a, b, expect) {
    const square = { x: piece.square.x + a, y: piece.square.y + b };
    while (inBounds(square)) {
        const target = game.pieces.find(({ x, y }) => square.x == x && square.y == y);
        if (target) {
            if (target.img.slice(0, 5) != piece.slice(0, 5) &&
                (target.img.slice(6) == "queen" || target.img.slice(6) == expect)) {
                return true;
            }
            return false;
        }
        square.x += a, square.y += b;
    }
    return false;
};

function isCheckInnerSingle(piece, a, b, expect) {
    const square = { x: piece.square.x + a, y: piece.square.y + b };
    const target = game.pieces.find(({ x, y }) => square.x == x && square.y == y);
    if (inBounds(square) && target) {
        if (target.img.slice(0, 5) != piece.slice(0, 5) &&
            target.img.slice(6) == expect) {
            return true;
        }
    }
    return false;
};

function isCheck(colour) {
    const piece = game.pieces[colour == "white" ? 12 : 28];
    const step = colour == "white" ? 1 : -1;
    return isCheckInner(piece, 1, 1, "bishop") ||
        isCheckInner(piece, -1, 1, "bishop") ||
        isCheckInner(piece, 1, -1, "bishop") ||
        isCheckInner(piece, -1, -1, "bishop") ||
        isCheckInner(piece, 1, 0, "rook") ||
        isCheckInner(piece, -1, 0, "rook") ||
        isCheckInner(piece, 0, 1, "rook") ||
        isCheckInner(piece, 0, -1, "rook") ||
        isCheckInnerSingle(piece, 1, 2, "knight") ||
        isCheckInnerSingle(piece, 1, -2, "knight") ||
        isCheckInnerSingle(piece, -1, 2, "knight") ||
        isCheckInnerSingle(piece, -1, -2, "knight") ||
        isCheckInnerSingle(piece, 2, 1, "knight") ||
        isCheckInnerSingle(piece, 2, -1, "knight") ||
        isCheckInnerSingle(piece, -2, 1, "knight") ||
        isCheckInnerSingle(piece, -2, -1, "knight") ||
        isCheckInnerSingle(piece, 1, step, "pawn") ||
        isCheckInnerSingle(piece, -1, step, "pawn");
}

function inBounds(square) {
    return square.x >= 0 && square.x <= 7 && square.y >= 0 && square.y <= 7;
}


setup()
window.onresize = () => render()
canvas.onclick = onclick
window.onclick = () => render()
game.socket.onopen = onopen
game.socket.onmessage = onmessage
