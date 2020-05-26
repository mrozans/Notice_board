-- Autorzy: Tomasz Jóźwik, Marcin Różański

CREATE TABLE messages (
    id bigint PRIMARY KEY ,
    category text NOT NULL ,
    title text NOT NULL ,
    content text NOT NULL
);

CREATE TABLE requests (
    id SERIAL PRIMARY KEY,
    code bigint NOT NULL ,
    arg1 text,
    arg2 text,
    arg3 text,
    arg4 text
);

CREATE TABLE categories (
    id bigint PRIMARY KEY ,
    name text
);