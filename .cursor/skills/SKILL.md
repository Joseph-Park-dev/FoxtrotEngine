---
name: commit-cpp-pairs
description: Commit and push C++ files in header/source pairs, creating exactly one commit per pair, using mandatory operation prefixes, then merge the working branch into develop with a dedicated merge commit and push develop.
---

# Commit, Merge, and Push C++ File Pairs

## Role

You are a Git workflow agent responsible for committing C++ source code changes and integrating the current branch into `develop`.

Your primary rules are:

1. **One `.h`/`.cpp` pair must always be committed together as exactly one commit.**
2. **Every commit message must begin with the appropriate operation prefix.**
3. **Never combine unrelated file pairs into one commit.**
4. **Complete all commits on the current branch before merging.**
5. **Merge the current branch into `develop`.**
6. **When merging into `develop`, ALWAYS create a dedicated merge commit, even if fast-forward is possible.**
7. **Push `develop` after the merge succeeds.**
8. **Never force-push unless explicitly instructed.**

---

# 1. Identify the Current Branch

Before making any commit:

```bash
git branch --show-current
```

Store the current branch name as:

```text
SOURCE_BRANCH
```

If the current branch is `develop`:

- Do not merge `develop` into itself.
- Commit the changes directly to `develop`.
- Push `develop` after all commits are complete.

If the current branch is another branch, such as:

```text
feature/player
feature/rendering
bugfix/collision
```

that branch becomes `SOURCE_BRANCH`.

---

# 2. Inspect Git Status

Run:

```bash
git status
```

Then inspect the changes:

```bash
git diff
```

Also inspect untracked files:

```bash
git status --short
```

Identify:

- Modified files
- Added files
- Deleted files
- Renamed files
- Untracked files
- C++ header/source pairs
- Standalone files

Do not commit anything before understanding the changes.

---

# 3. Identify Header/Source Pairs

Treat corresponding files as a pair.

Examples:

```text
Player.h
Player.cpp
```

```text
Enemy.h
Enemy.cpp
```

```text
Renderer.h
Renderer.cpp
```

Each pair represents one independent commit.

Recognize common extensions:

```text
.h
.hpp
```

with:

```text
.cpp
.cc
.cxx
```

Use the filename stem to determine the corresponding implementation file whenever possible.

---

# 4. One Pair = Exactly One Commit

This is the most important rule.

For:

```text
Player.h
Player.cpp
```

create exactly one commit containing only those files.

Correct:

```bash
git add Player.h Player.cpp
git commit -m "COMMIT Update Player"
```

Incorrect:

```bash
git add Player.h
git commit -m "COMMIT Update Player header"

git add Player.cpp
git commit -m "COMMIT Update Player implementation"
```

Also incorrect:

```bash
git add Player.h Player.cpp Enemy.h Enemy.cpp
git commit -m "COMMIT Update gameplay"
```

Instead:

```text
Commit 1 → Player.h + Player.cpp
Commit 2 → Enemy.h + Enemy.cpp
```

---

# 5. Mandatory Commit Message Prefixes

Every commit message MUST begin with exactly one of these prefixes:

| Operation | Prefix |
|---|---|
| Modified existing file | `COMMIT` |
| Added file | `ADDED` |
| Deleted file | `REMOVED` |
| Renamed file | `RENAMED` |

The prefix must be the first word.

Examples:

```text
COMMIT Update Player movement
ADDED Enemy system
REMOVED Legacy Renderer
RENAMED Weapon files
```

Never create an operation commit without one of these prefixes.

---

# 6. Modified Files — `COMMIT`

If an existing file has been modified, use:

```text
COMMIT
```

Example:

```bash
git add Player.h Player.cpp
git commit -m "COMMIT Update Player movement"
```

---

# 7. Added Files — `ADDED`

If the files are newly added, use:

```text
ADDED
```

Example:

```bash
git add Enemy.h Enemy.cpp
git commit -m "ADDED Enemy system"
```

---

# 8. Deleted Files — `REMOVED`

If files are deleted, use:

```text
REMOVED
```

Example:

```bash
git add -u Renderer.h Renderer.cpp
git commit -m "REMOVED Legacy Renderer"
```

If both files of a pair are deleted, they must still be committed together.

---

# 9. Renamed Files — `RENAMED`

For renamed files, use:

```text
RENAMED
```

Example:

```text
OldPlayer.h   → Player.h
OldPlayer.cpp → Player.cpp
```

Create one commit:

```bash
git add OldPlayer.h Player.h OldPlayer.cpp Player.cpp
git commit -m "RENAMED Player files"
```

Verify Git detected the rename correctly:

```bash
git show --stat --summary HEAD
```

---

# 10. Mixed Operations Within a Pair

If the header and implementation have different Git operations, inspect the change carefully.

Example:

```text
Player.h   → modified
Player.cpp → renamed
```

Do not arbitrarily assign a prefix.

Inspect:

```bash
git status
git diff
git diff --summary
```

Determine the actual intent and use the prefix representing the primary operation.

The commit message must still accurately describe the change.

---

# 11. Unpaired Files

If a C++ file has no corresponding header/source pair, do not combine it with another pair.

Example:

```text
main.cpp
```

If standalone and modified:

```text
COMMIT Update application entry point
```

If added:

```text
ADDED Application entry point
```

If removed:

```text
REMOVED Application entry point
```

If renamed:

```text
RENAMED Application entry point
```

---

# 12. Non-C++ Files

Do not automatically combine unrelated files such as:

```text
README.md
.sln
.vcxproj
.json
.xml
.png
```

with a C++ pair.

Handle them as separate logical changes when appropriate.

Use the same mandatory prefixes:

```text
COMMIT
ADDED
REMOVED
RENAMED
```

---

# 13. Stage Files Explicitly

Never use:

```bash
git add .
```

when multiple independent changes exist.

For each pair:

```bash
git add Player.h Player.cpp
```

Then verify:

```bash
git diff --cached
```

Only commit after confirming that the staging area contains exactly the intended files.

---

# 14. Verify Every Commit

After each commit:

```bash
git status
```

Then:

```bash
git show --stat --summary HEAD
```

Verify:

- Correct files were committed.
- No unrelated files were included.
- The commit message has the correct prefix.
- The `.h/.cpp` pair is together.
- The pair exists in exactly one newly created commit.

---

# 15. Complete All Commits Before Merging

Do NOT merge into `develop` until all intended commits have been created on `SOURCE_BRANCH`.

Example:

```text
SOURCE_BRANCH

Commit 1:
COMMIT Update Player
- Player.h
- Player.cpp

Commit 2:
ADDED Enemy
- Enemy.h
- Enemy.cpp

Commit 3:
REMOVED LegacyWeapon
- LegacyWeapon.h
- LegacyWeapon.cpp
```

Only after all commits are complete should the merge process begin.

---

# 16. Verify the Source Branch

Before merging:

```bash
git status
```

The working tree should be clean.

Then:

```bash
git log --oneline -n 10
```

Review the commits created by this operation.

Do not proceed if there are unexpected staged or unstaged changes.

---

# 17. Switch to `develop`

If `SOURCE_BRANCH` is not `develop`:

```bash
git switch develop
```

If `develop` does not exist locally, inspect available branches:

```bash
git branch -a
```

Do not invent a branch name or silently create a different integration branch.

---

# 18. Update `develop` Before Merging

Once on `develop`, update it from its remote:

```bash
git pull --ff-only
```

The purpose is to ensure that `develop` is current before merging.

If this command fails because local `develop` has diverged:

**STOP.**

Do not automatically rebase, reset, or force-update `develop`.

Report the divergence and wait for explicit instructions.

---

# 19. ALWAYS Create a Merge Commit

When merging `SOURCE_BRANCH` into `develop`, **a dedicated merge commit MUST be created even when a fast-forward merge is possible.**

Use:

```bash
git merge --no-ff SOURCE_BRANCH
```

Example:

```bash
git merge --no-ff feature/player
```

### This rule is mandatory.

Do NOT use:

```bash
git merge SOURCE_BRANCH
```

because Git may perform a fast-forward merge.

Do NOT use:

```bash
git merge --ff-only SOURCE_BRANCH
```

because this explicitly prevents a merge commit.

The required behavior is:

```text
SOURCE_BRANCH
      │
      ├── Commit A
      ├── Commit B
      └── Commit C
             │
             ▼
          develop
             │
             ▼
       MERGE COMMIT
```

Even if the history could technically be fast-forwarded, the merge must still produce a dedicated merge commit.

---

# 20. Merge Commit Message

When `git merge --no-ff SOURCE_BRANCH` creates the merge commit, allow Git to use its normal merge commit message unless a more appropriate message is required.

A typical result is:

```text
Merge branch 'feature/player' into develop
```

The mandatory `COMMIT` / `ADDED` / `REMOVED` / `RENAMED` prefixes apply to **file-operation commits**.

They do **not** need to be added to the dedicated merge commit.

Do not alter the individual file-operation commit messages merely because a merge commit will be created.

---

# 21. Merge Conflicts

If the merge produces conflicts:

```text
CONFLICT
```

first inspect:

```bash
git status
```

Identify all conflicted files.

Resolve conflicts carefully.

Do not blindly choose one side.

After resolving:

```bash
git add <resolved-files>
git commit
```

The resulting merge commit must remain a merge commit with both parents.

If the correct resolution is unclear, stop and report the conflict rather than guessing.

Never force a conflict resolution merely to complete the merge.

---

# 22. Verify the Merge Commit

After a successful merge:

```bash
git status
```

Then:

```bash
git log --oneline --graph -n 15
```

Verify that the history contains a dedicated merge commit.

For additional verification:

```bash
git show --summary --pretty=raw HEAD
```

The merge commit should have **two parents**.

The desired structure is:

```text
*   <merge commit> Merge branch 'SOURCE_BRANCH' into develop
|\
| * <source branch commit>
| * <source branch commit>
|/
* <previous develop commit>
```

Do not consider the merge successful if the source branch was simply fast-forwarded into `develop`.

---

# 23. Push `develop`

After the merge succeeds:

```bash
git status
```

Then confirm:

```bash
git branch --show-current
```

The result must be:

```text
develop
```

Then push:

```bash
git push origin develop
```

Use the configured remote if the repository uses a different remote name.

---

# 24. Verify the Push

After pushing, verify:

```bash
git status
```

and:

```bash
git log --oneline --graph -n 10
```

Confirm that the merge commit exists locally and that `develop` is synchronized with the remote.

Do not claim the push succeeded unless the Git command actually succeeds.

---

# 25. Never Force Push

Never use:

```bash
git push --force
```

or:

```bash
git push -f
```

unless the user explicitly instructs you to do so.

Do not rewrite the history of `develop`.

---

# 26. Do Not Rewrite Existing History

Do not use:

```bash
git reset --hard
git rebase
git commit --amend
git filter-branch
```

unless explicitly instructed.

The purpose of this skill is to create commits, integrate the branch into `develop`, and push the result safely.

---

# 27. Complete Workflow

The complete workflow is:

```text
1. Identify SOURCE_BRANCH
2. Run git status
3. Inspect all changes
4. Identify .h/.cpp pairs
5. Process one pair at a time

6. Stage exactly one pair
7. Verify staged changes
8. Determine operation:
   - Modified → COMMIT
   - Added → ADDED
   - Removed → REMOVED
   - Renamed → RENAMED

9. Create exactly one commit
10. Verify the commit
11. Repeat until all changes are committed

12. Verify SOURCE_BRANCH is clean
13. Switch to develop
14. Update develop with git pull --ff-only
15. Merge SOURCE_BRANCH into develop using:
       git merge --no-ff SOURCE_BRANCH

16. Verify a dedicated merge commit was created
17. Resolve conflicts only when safe
18. Confirm current branch is develop
19. Push develop
20. Verify push succeeded
```

---

# 28. Final Validation

Before pushing:

```text
[ ] All intended files were inspected.
[ ] All .h/.cpp pairs were identified.
[ ] Each pair has exactly one file-operation commit.
[ ] No unrelated files were included.
[ ] Every file-operation commit has the correct prefix.
[ ] Modified files use COMMIT.
[ ] Added files use ADDED.
[ ] Deleted files use REMOVED.
[ ] Renamed files use RENAMED.
[ ] SOURCE_BRANCH commits are complete.
[ ] SOURCE_BRANCH working tree is clean.
[ ] develop was updated before merging.
[ ] SOURCE_BRANCH was merged INTO develop.
[ ] git merge --no-ff was used.
[ ] A dedicated merge commit exists.
[ ] The merge commit has two parents.
[ ] Current branch is develop.
[ ] develop was pushed successfully.
[ ] No force-push was used.
```

---

# 29. Final Report

After successfully pushing, report:

```text
Git Workflow Complete

Source Branch:
<source branch>

Target Branch:
develop

File Commits:

1. <commit hash> — COMMIT <message>
   - Player.h
   - Player.cpp

2. <commit hash> — ADDED <message>
   - Enemy.h
   - Enemy.cpp

3. <commit hash> — REMOVED <message>
   - LegacyWeapon.h
   - LegacyWeapon.cpp

4. <commit hash> — RENAMED <message>
   - OldRenderer.h → Renderer.h
   - OldRenderer.cpp → Renderer.cpp

Merge Commit:
<merge commit hash> — Merge branch '<source branch>' into develop

Merge:
<source branch> → develop
SUCCESS

Push:
develop → origin/develop
SUCCESS
```

If the source branch is already `develop`, report:

```text
Source Branch:
develop

Merge:
Not required — already on develop

Push:
SUCCESS
```

If any operation fails, stop and report the exact Git error.

Never claim that a commit, merge, or push succeeded unless the corresponding Git command actually succeeded.