# Flashcards Application

This is a simple Flashcards application built using Qt. The application allows users to manage decks of flashcards, add new decks, and remove existing ones. Each deck is displayed in a grid layout within a scrollable area to accommodate any number of decks.

## Features

- **Grid Layout**: Decks are displayed in a grid layout, starting from the top-left corner, filling each row before moving to the next.
- **Scroll Area**: The main window features a scrollable area to handle numerous decks, ensuring a seamless user experience.
- **Dynamic Deck Naming**: Users can add new decks with custom names prompted through an input dialog.
- **Toolbar for Deck Management**: A toolbar is provided with buttons for adding and removing decks.
- **Database Integration**: Decks are stored in a PostgreSQL database, ensuring data persistence and easy retrieval.

## MainWindow Class

### Constructor

- **Initialization**: The `MainWindow` class initializes with a fixed size and sets up the central widget and layout structure.
- **Scroll Area**: A `QScrollArea` is used to provide a scrollable view of the decks.
- **Container Widget**: A container widget within the scroll area holds a `QGridLayout` to manage the deck widgets.
- **Grid Layout**: The `QGridLayout` ensures decks are positioned in a structured manner, with a fixed number of columns.
- **Toolbar**: A toolbar is created with buttons for adding and removing decks, connected to their respective slots.

### Adding a Deck

- **User Prompt**: When adding a new deck, the user is prompted to enter a custom name for the deck via a `QInputDialog`.
- **Deck Widget**: A new `QLabel` is created for each deck, displaying the name provided by the user.
- **Grid Positioning**: The new deck is added to the grid layout at the current row and column position. The position is updated to ensure a structured layout.
- **Database Insertion**: The new deck is also added to the PostgreSQL database, ensuring data persistence.

### Removing a Deck

- **Removing Logic**: Users can remove a deck by clicking the remove button on the toolbar. The layout is updated accordingly.
- **Database Deletion**: The removed deck is deleted from the PostgreSQL database to keep the data synchronized.

### Load Decks from Database

- **Database Retrieval**: Decks are loaded from the PostgreSQL database when the application starts.
- **Grid Display**: Loaded decks are displayed in the grid layout within the scrollable area.
- **Deck Mapping**: A map is maintained to keep track of deck widgets and their IDs for easy removal.

### Database Management

- **Connection Handling**: The application connects to the PostgreSQL database, ensuring the connection is successfully established before performing any operations.
- **Query Execution**: SQL queries are executed to create tables, insert decks, and retrieve decks.
- **Error Handling**: Detailed error messages are logged to help identify and troubleshoot issues during database operations.


## Future Enhancements

- **Flashcard Management**: Adding functionality to manage individual flashcards within each deck.
- **Enhanced UI**: Improving the user interface with additional features and better styling.
- **Create Language Exercises**: User is prompted to fill in the gap with the correct answear based on his current deck


