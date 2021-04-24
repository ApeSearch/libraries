# ApeSearch
Our project is split into 5 main repositories:

 - backend
	 - Provides the infrastructure for receiving user queries, distributing them to all the ApeMachines, returning the top 10 results from each, and merging them into a single list to return to the frontend

- frontend
	- Creates a multithreaded socket server that listens on port 80 and serves a static HTML page back to the client

- crawler
	- Provides infrastructure for starting up a web crawler that uses a seed list to crawl, parse, and store HTML webpages

- query-constraint
	- Provides the ability to build up serialized inverted index chunks (ApeChunks) from processed HTML files. Also provides a search **query compiler**, **constraint solver** that finds matches for that query in any number of ApeChunks, and a **ranker** that ranks matches. See query-constraint README for a more in-depth overview

- libraries
	- Contains useful helper code and our own libraries such as string.h, vector.h, unique_ptr.h, etc... Also provides a unit test framework for use in all the other repositories. 
