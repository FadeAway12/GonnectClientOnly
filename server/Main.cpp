#include "crow.h"
#include "crow/middlewares/cors.h"

#include "Board.h"

int main()
{
    crow::App<crow::CORSHandler> app;

    auto &cors = app.get_middleware<crow::CORSHandler>();
    cors.global()
        .origin("http://localhost:3000") // or .origin("*")
        .methods("GET"_method, "POST"_method, "OPTIONS"_method)
        .headers("Content-Type");

    Board b{5};

    CROW_ROUTE(app, "/")([]()
                         { return "Hello world"; });

    CROW_ROUTE(app, "/board").methods("GET"_method)([&b]
                                                    { return b.boardJson(); });

    CROW_ROUTE(app, "/isConnected").methods("GET"_method)([&b]
                                                          { return b.isConnectedJson(); });

    CROW_ROUTE(app, "/bestMove").methods("GET"_method)([&b](const crow::request &req)
                                                       { 
        auto queries = crow::query_string{ req.url_params };
    int player = 1; 
    if (queries.get("player")) {
        player = std::stoi(queries.get("player"));
    }

    return b.bestMoveJson((Stone)player); });

    CROW_ROUTE(app, "/postBoard").methods("POST"_method)([&b](const crow::request &req)
                                                         {
        auto data = crow::json::load(req.body);
    if (!data) {
        return crow::response(400, "improper format of board");
        }

    const auto& array = data["array"];
        std::cout << "TEST TEST: " << array << std::endl;

        b.setBoard(array);

        crow::json::wvalue res;
        res["status"] = 200;
        return crow::response{ res }; });

    CROW_ROUTE(app, "/postColor").methods("POST"_method)([&b](const crow::request &req)
                                                         {
        auto data = crow::json::load(req.body);
    if (!data) {
        return crow::response(400, "improper format of board");
    }

    int ind = data["ind"].i();
    Stone color = (Stone)data["color"].i();
    
    b.setColor(ind/b.getSize(), ind% b.getSize(), color);

    crow::json::wvalue res;
    res["status"] = 200;
    return crow::response{ res }; });

    app.port(18080).run();
}

crow::json::wvalue Board::boardJson() const
{ // will improve later.
    crow::json::wvalue::list rows;
    for (int i = 0; i < size; i++)
    {
        crow::json::wvalue::list row;
        for (int j = 0; j < size; j++)
        {
            row.push_back(board[i * size + j]);
        }
        rows.push_back(row);
    }
    crow::json::wvalue out;
    out["board"] = std::move(rows);
    out["player"] = counter % 2 == 0 ? 1 : 2; // ADDED RECENTLY. EASIER TRACKING OF PLAYER!
    return out;
}

void Board::setBoard(crow::json::rvalue newBoard)
{
    prev = board;

    if (newBoard.size() != board.size())
    {
        std::cerr << "warning, mutating board size. could be error";
    }

    board.clear();
    size = sqrt(newBoard.size());

    for (int i = 0; i < newBoard.size(); ++i)
    {
        if (newBoard[i].t() != crow::json::type::Number)
        {
            std::cerr << "WARNING! VAL MIGHT NOT BE A INT!";
            continue;
        }
        board.push_back(Stone(newBoard[i].i()));
    }
}

crow::json::wvalue Board::bestMoveJson(Stone player) const
{
    int ind = bestMove(player);
    crow::json::wvalue out;
    out["bestMove"] = std::move(ind);
    return out;
}

crow::json::wvalue Board::isConnectedJson() const
{
    crow::json::wvalue out;
    if (isConnected(Stone::Black))
        out["isConnected"] = std::move(1);
    else if (isConnected(Stone::White))
        out["isConnected"] = std::move(2);
    else
        out["isConnected"] = std::move(0);
    return out;
}