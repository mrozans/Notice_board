-- Autorzy: Tomasz Jóźwik, Marcin Różański
-- Data: 25.05.2020
-- Tytuł projektu: Tablica ogłoszeń

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