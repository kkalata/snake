#include "snake.h"

void UnsetBestPlayers(
    BestPlayers *const bestPlayers
)
{
    bestPlayers->newBestPlayerI = BEST_PLAYER_COUNT;
    bestPlayers->listUpdated = 0;
    for (int bestPlayerI = 0; bestPlayerI < BEST_PLAYER_COUNT; bestPlayerI++)
    {
        bestPlayers->list[bestPlayerI].pointsScored = 0;
        bestPlayers->list[bestPlayerI].playerName = NULL;
    }
}

void LoadBestPlayers(
    BestPlayers *const bestPlayers
)
{
    UnsetBestPlayers(bestPlayers);

    FILE *bestPlayersFile = fopen(BEST_PLAYERS_FILEPATH, "r");

    if (bestPlayersFile == NULL)
    {
        return;
    }
    // check if the file is empty
    if (fgetc(bestPlayersFile) == EOF)
    {
        fclose(bestPlayersFile);
        return;
    }
    rewind(bestPlayersFile);
    for (int bestPlayerI = 0; bestPlayerI < BEST_PLAYER_COUNT; bestPlayerI++)
    {
        if (feof(bestPlayersFile))
        {
            break;
        }
        bestPlayers->list[bestPlayerI].playerName = (char *) malloc(BEST_PLAYER_NAME_MAX_LENGTH * sizeof(char));
        fscanf(
            bestPlayersFile,
            "%u %[^\n]\n",
            &bestPlayers->list[bestPlayerI].pointsScored,
            bestPlayers->list[bestPlayerI].playerName
        );
    }

    fclose(bestPlayersFile);
}

void SaveBestPlayers(
    const BestPlayer bestPlayers[]
)
{
    FILE *bestPlayersFile = fopen(BEST_PLAYERS_FILEPATH, "w");
    if (bestPlayersFile == NULL)
    {
        return;
    }

    for (int bestPlayerI = 0; bestPlayerI < BEST_PLAYER_COUNT; bestPlayerI++)
    {
        if (bestPlayers[bestPlayerI].playerName == NULL)
        {
            break;
        }
        fprintf(
            bestPlayersFile,
            "%u %s\n",
            bestPlayers[bestPlayerI].pointsScored,
            bestPlayers[bestPlayerI].playerName
        );
    }

    fclose(bestPlayersFile);
}

void PrepareNewBestPlayer(
    BestPlayers *const bestPlayers,
    const Uint32 pointsScored
)
{
    int newBestPlayerI = 0;
    BestPlayer *newBestPlayer;
    for (newBestPlayerI; newBestPlayerI < BEST_PLAYER_COUNT; newBestPlayerI++)
    {
        newBestPlayer = &bestPlayers->list[newBestPlayerI];
        if (pointsScored >= newBestPlayer->pointsScored)
        {
            break;
        }
    }
    if (newBestPlayerI == BEST_PLAYER_COUNT) // player outside of list
    {
        return;
    }
    free(bestPlayers->list[BEST_PLAYER_COUNT - 1].playerName);
    for (int bestPlayerI = BEST_PLAYER_COUNT - 2; bestPlayerI >= newBestPlayerI; bestPlayerI--)
    {
        bestPlayers->list[bestPlayerI + 1].pointsScored = bestPlayers->list[bestPlayerI].pointsScored;
        bestPlayers->list[bestPlayerI + 1].playerName = bestPlayers->list[bestPlayerI].playerName;
    }
    
    newBestPlayer->pointsScored = 0;
    newBestPlayer->playerName = (char *) malloc(BEST_PLAYER_NAME_MAX_LENGTH * sizeof(char));
    memset(newBestPlayer->playerName, '\0', BEST_PLAYER_NAME_MAX_LENGTH);

    bestPlayers->newBestPlayerI = newBestPlayerI;
    newBestPlayer->pointsScored = pointsScored;
}

void AppendNewBestPlayerName(
    BestPlayers *const bestPlayers,
    const char *text
)
{
    strcat(
        bestPlayers->list[bestPlayers->newBestPlayerI].playerName,
        text
    );
}

void DestroyBestPlayersList(
    BestPlayer bestPlayers[]
)
{
    for (int bestPlayerI = 0; bestPlayerI < BEST_PLAYER_COUNT; bestPlayerI++)
    {
        free(bestPlayers[bestPlayerI].playerName);
    }
}
