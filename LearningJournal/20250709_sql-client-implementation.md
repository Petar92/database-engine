# Journal Entry - July 9th, 2025

I have continued thinking and reading about how to parse the queries on the client and then how to group that information and send it to the server. I have decided to use a simple structure to hold the parsed query information. 
What I concluded is that I can just parse the query normally, I will not be writting my onw parser since I have already spent too much time on side quests, instead I will just use the libpg_query library to do this and send the AST to the server where it will be passed on to the data system binder (pretty much checks if the stuff from the AST is legit/exists in the database, like tables etc) which then generates the logical plan which is then passed to the optimizer but this fun stuff will come later.
For now, I will just focus on the client and how to send the parsed query to the server. In order to do that, I will use the boost asio library to create a simple TCP client, and as I have already mentioned I will use the libpg_query library. This shouldn't take too much time.
I have also decided to delete everything from this repo and start over as it's a complete messs right now, and very few things are actually useful.
I will build the client in a separate repo.
