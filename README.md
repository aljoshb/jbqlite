# Introduction

jbqlite is a a small, zero-configuration, custom-tailored, embeddable, thread-safe, easily maintainable, transaction-oriented, SQL-based, relational database management system, that stores an entire databse in a single file that holds all tables and indexes. jbqlite was built as a simplified version of sqlite as it also organizes all tables into seperate B* trees and indexes into B-tress. It supports the core transaction properties, namely, atomicity, consistency, isolation and durability.

## Running jbqlite locally

I created a command line REPL interface for the database which you can interact with and use to store data. I wrote up a Makefile that will build the database. To get started, clone this repo, and cd into the root of the repo and type ```make```. This will build the database and create the ```jbqlite``` program. Next, to get into the REPL mode, type the following command ```./jbqlite <a database file name>``` and you will be in the REPL mode.

The database currently supports inserts and selects into a fixed table schema that has an id, username and email address. To insert into the database, you can type the following command: ```insert 1 jon jondoe@abc.com```. You can do several inserts and to print out the table, you can type the following command: ```select```.

To exit from the REPL mode, type ```.exit```. Once you re-enter the REPL mode you will see that you data was persisted and still exists.

## Inspiration

This project was inspired by desire to learn how a database works internally to persist and retrieve data. This project was built following the guide of the [SQLite Database System Design and Implementation](https://books.google.ca/books?id=OEJ1CQAAQBAJ&printsec=copyright&redir_esc=y#v=onepage&q&f=false) book.