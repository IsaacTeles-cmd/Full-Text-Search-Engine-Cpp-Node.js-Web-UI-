const express = require("express");
const { execFile } = require("child_process");
const path = require("path");
const { stdout } = require("process");

const app = express();
const PORT = 3000;

app.use(express.static(__dirname));
// Abs path to bin.
const enginePath = path.join(__dirname, "../bin/search_engine");

app.get("/search", (req, res) =>{
    const query = req.query.q;


    // Input validation
    if(!query || query.trim() === ""){
        return res.status(400).json({ error: "Missing or empty query" });
    }

    /**
     * Using an execfile to avoid headaches with shells opening due to
     *  an exec command and command injection happening... 
     * for God's sake!
     */
    execFile(enginePath, [query],{ cwd: path.join(__dirname, "..") }, (error, stdout, stderr) =>{
        if(error){
            console.error(`Execution Error: ${stderr}`);
            return res.status(500).json({
                error: "Search engine failed",
                details: stderr
            });
        }
        try{
            // Attempts to convert C++ output to JSON
            const data = JSON.parse(stdout);
            res.json(data);
        } catch(parseError){
            console.error("Invalid output from engine.", stdout);
            res.status(500).json({
                error: "Internal processing error",
                message: "Engine output was not a valid JSON"
            });
        }
    });
});

app.get("/autocomplete", (req, res) =>{
    const query = req.query.q;

    if(!query) return res.json([]);

    execFile(enginePath, ["--autocomplete", query], { cwd: path.join(__dirname, "..") }, (error, stdout)=>{
        if(error) return res.json([]);

        try{
            const data = JSON.parse(stdout);
            res.json(data);
        } catch { 
            res.json([]);
        }
    });
});

app.post("/reindex", (req, res)=>{
    execFile(enginePath, 
        ["reindex"],{
        cwd: path.join(__dirname, "..") // Reindex directory path forced.
        }, (error, stdout, stderr)=>{
        console.log("REINDEX STDOUT:", stdout)
        console.log("REINDEX STDERR:", stderr);

        if(error){
            console.error("REINDEX ERROR:", error);
            return res.status(500).json({ error: "Reindex failed" });
        }

        res.json({ status: "ok"});
    });
});

app.listen(PORT, () =>{
    console.log(`[SERVER] Running on http://localhost:${PORT}`);
    console.log(`[CONFIG] Engine path: ${enginePath}`);
});
