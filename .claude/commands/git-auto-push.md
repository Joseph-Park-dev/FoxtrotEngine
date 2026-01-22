Review all local modifications relative to HEAD, including both staged and unstaged changes.

Before adding, check for any of the following patterns:

The file must be located in "src" folder. Do not add and commmit files in include.

Before committing, check for any of the following patterns:

COMMIT, ADD, REMOVED, or similar developer notes.

Group local modifications in categories and create commit for each categories.

Output a short confirmation message indicating success (e.g., “✅ Commit completed — working tree clean.”).

If there is no commit, write a short confirmation message indicating no commit.

Checkout to branch "develop" and merge the previous branch into branch "develop".
Create a new commit even if fast-forward is possible.

If conflict is detected, output a confirmation message indicating conflict, and wait for additional command.

If merging is successful, output a confirmation message indicating success (e.g., “✅ Merging completed to branch develop.”).

After merging, push branch "develop".

If pushing is unsuccessful, output a confirmation message indicating failure, listing the problematic file, and wait for additional command.

If pushing is successful, output a confirmation message indicating success (e.g., “✅ Pushing completed.”).