//  Harbor - A portable and highly customisable game engine
//  Copyright (C) 2021 Semphris <semphris@protonmail.com>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as
//  published by the Free Software Foundation, either version 3 of the
//  License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef _HEADER_HARBOR_UTIL_LOG_HPP
#define _HEADER_HARBOR_UTIL_LOG_HPP

#include <iostream>

// MSVC defines an "ERROR" macro, which conflicts with the enum below.
#ifdef ERROR
#undef ERROR
#endif

#define log_(LEVEL) if (Log::s_level >= Log::Level::LEVEL)                     \
                    Log::log(#LEVEL, __FILE__, __LINE__)

/**
 * A log is DEBUG importance if it merely prints information about what the
 * program is doing.
 *
 * Example: Use this to print the OpenGL version used by the program. Since
 *          printing general information isn't a direct message from the
 *          original developer to maintainers that something might go wrong, it
 *          should be categorized as debug. It could mean something went
 *          wrong if the maintainer wanted a specific OpenGL version, but
 *          unless the original developer also expects it, it should be
 *          debug.
 */
#define log_debug log_(DEBUG)

/**
 * A log is INFO importance if it prints something that might be a mistake, but
 * for which it is possible to imagine a reasonable scenario in which this point
 * in the code is reached.
 *
 * Example: Use this to signal that a UI panel/container is empty upon
 *          performing actions which would normally need children. It might be
 *          intended by the next developer, for example if the contents of the
 *          container is dynamic; but it might also mean the developer forgot to
 *          fill the container with its contents (or filled the wrong one).
 *          Since it's probably not a mistake, it only has info status.
 */
#define log_info  log_(INFO)

/**
 * A log is WARN importance if it probably means another developer made a
 * mistake, but which won't prevent the normal execution of the program.
 *
 * Example: Use this when the internal state of an object is invalid but
 *          recoverable. (For example, calling `set_mystate()` with 6 for a
 *          state that can hold values in an int from 0 to 3 and -1 for "none";
 *          since 6 is out of the -1 to 3 range, set m_mystate to -1 and do a
 *          `log_warn << "Inavlid state: " << state << std::endl`).
 */
#define log_warn  log_(WARN)

/**
 * A log is ERROR importance if some part of the program will not work as
 * expected, but that does not require the program to crash. This is the lowest
 * level where the end user is guaranteed notice that something has gone wrong
 * in a way that is directly related to the logged event.
 *
 * Example: Use this when you can't load a texture that is meant to be drawn,
 *          but an alternative "error"-like texture can be used instead. The
 *          user will notice it, but the program can keep running.
 *
 * Example: Use this in videogames when a level that the user requested to play
 *          can't be loaded. The user won't be able to play the level, but
 *          instead of crashing the program, they can simply be sent back to the
 *          menu with an in-game error message.
 */
#define log_error log_(ERROR)

/**
 * A log is FATAL importance if the program is guaranteed to quit as a result
 * of the corresponding point in the code being reached.
 *
 * Example: Use this when you can't initialise essential subcomponents (e. g.
 *          SDL, if your program can't run without it).
 *
 * Example: It may also be used it if the user specifically invoked the program
 *          to perform a specific task that can't be accomplished; for example,
 *          running an image editor with CLI arguments pointing to a
 *          non-existent file. That won't necessarily crash the program if it
 *          correctly performs checks, but the program won't be able to perform
 *          its task and will have to quit, as it has nothing else to do for its
 *          execution. If you choose that in such circumstances, the image
 *          editor should stay open and offer to open a different file, use
 *          log_error and graphical interfaces to alert the user.
 */
#define log_fatal log_(FATAL)

/**
 * Class containing functions relevant to logging. Do not use directly, please
 * use the macros instead (log_fatal, log_error, log_warn, log_info, log_debug).
 *
 * How to know which logging to use:
 * - Is it information intended for developers, maintainers, contributors,
 *   sysadmins, people who will port the program to another system, etc., or is
 *   it intended for the users of the program?
 *
 *     For the users:   Please don't use logging for this unless the app is CLI;
 *                      many platforms don't provide obvious CLI interfaces for
 *                      GUI programs.
 *     For the devs:    Continue below.
 *
 * - Upon reaching this point in the code, will the program be forced to quit?
 * 
 *     Yes:             Use `log_fatal << "Message here << std::endl`.
 *     No:              Continue below.
 *
 * - Upon reaching this point in the code, is it 100% guaranteed some part of
 *   the program won't work properly?
 *
 *     Yes:             Use `log_error << "Message here << std::endl`.
 *     No:              Continue below.
 *
 * - Let's work the other way now: is it a message that means it's possible that
 *   something was unintended, or merely printing the internal status of the
 *   program?
 *
 *     Internal status: Use `log_debug << "Message here << std::endl`.
 *     Might be an err: Continue below.
 *
 * - Can you imagine a plausible, reasonable scenario where a well-built program
 *   will knowingly reach this point in the code under certain circumstances?
 *
 *     Yes:             Use `log_info << "Message here << std::endl`.
 *     No:              Use `log_warn << "Message here << std::endl`.
 *
 * NOTE: Also check the documentation of each of the 5 macros for examples of
 *       scenario along with an explanation about why the examples match their
 *       respective log levels.
 */
class Log final
{
public:
  enum class Level {
    NONE,
    FATAL,
    ERROR,
    WARN,
    INFO,
    DEBUG,
    ALL = DEBUG
  };

public:
  static std::ostream* s_log;
  static Level s_level;
  static std::ostream& log(const char* level, const char* file, int line);
};

#endif
