#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termio.h>
#include <time.h>
#include <unistd.h>

#define LETTER_COUNT 26
#define LETTER_LOOP 20
#define WORDS_COUNT 100
#define WORDS_LOOP 20
#define SHORTS_COUNT 30
#define SHORTS_LOOP 5
#define LONGS_COUNT 5

int getch() {
    int c;
    struct termios old;
    struct termios new;

    tcgetattr(0, &old);  // 원본 설정 old 구조체로 저장
    new = old;
    new.c_lflag &= ~(ICANON | ECHO);  // ICANON 비활성화 -> 버퍼 없이 즉시 입력 받도록 설정, ECHO 비활성화 -> 입력한 문자 화면에 표시하지 않도록 설정
    new.c_cc[VMIN] = 1;               // 최소 한 개의 문자가 입력될 때 까지 read() 반환하지 않음
    new.c_cc[VTIME] = 0;              // read() 호출의 타임아웃을 0으로 설정 (비활성화)

    tcsetattr(0, TCSAFLUSH, &new);  // new 구조체로 설정 적용
    c = getchar();                  // 문자 한개 입력 받음
    tcsetattr(0, TCSAFLUSH, &old);  // old 구조체로 설정 복원

    return c;
}

// 정확도를 계산해서 반환한다.
static inline int calculateAccuracy(int correct, int total) {
    if (correct == 0) return 0;
    return (double)correct / total * 100;
}

// 현재타수를 계산해서 반환한다.
static inline int calculateCpm(int correct, time_t start) {
    if (time(NULL) - start < 1) return 0;
    return correct /  ((double)(time(NULL) - start) / 60);
}

// 자리연습 상단 텍스트를 업데이트 한다.
static inline void updateLetterTexts(int progress, int wrong, int accuracy, char target) {
    system("clear");
    printf(">> 영문 타자 연습 프로그램: 자리 연습 <<\n");
    printf("진행도: %d%%\t 오타수: %d\t 정확도: %d%%\n\n", progress, wrong, accuracy);
    printf("%c", target);
}

// 단어연습 상단 텍스트를 업데이트 한다.
static inline void updateWordTexts(int progress, int wrong, int accuracy, char word[]) {
    system("clear");
    printf(">> 영문 타자 연습 프로그램 : 낱말 연습 <<\n");
    printf("진행도 : %d%%\t오타수 : %d\t정확도 : %d%%\n\n", progress, wrong, accuracy);
    printf("%s\n", word);
}

// 짧은 글 연습 상단 텍스트를 업데이트 한다. 유저가 현재까지 입력 한 글도 출력한다.
static inline void updateShortTexts(int progress, int cpm, int maxCpm, int accuracy, char target[], char input[]) {
    system("clear");
    printf(">> 영문 타자 연습 프로그램: 짧은 글 연습 <<\n");
    printf("진행도 : %d%%\t현재타수 : %d\t최고타수 : %d\t정확도 : %d%%\n\n", progress, cpm, maxCpm, accuracy);
    printf("%s\n", target);
    printf("%s", input);
}

// 긴 글 연습 상단 텍스트를 업데이트 한다.
static inline void updateLongTexts(int accuracy, int cpm, int line, char targetText[5][100], char input[5][100]) {
    system("clear");
    printf(">> 영문 타자 연습 프로그램: 긴 글 연습 <<\n");
    printf("정확도 : %d%%\t현재타수 : %d\n\n", accuracy, cpm);

    // 5행 이상 입력하면 두 번째 페이지의 다섯 문장을 보여주기
    if (line < 5) {
        for (int i = 0; i < 5; i++)
            printf("%s\n", targetText[i]);
    } else if (line < 10) {
        for (int i = 5; i < 10; i++)
            printf("%s\n", targetText[i]);
    }

    // 사용자 입력을 출력
    for (int i = 0; i <= line%5; i++) // line%5 => 0~4, 5~9를 한번에 표현
        printf("\n%s", input[i]);
}

// 자리 연습
void letterTyping() {
    int progress = 0, correct = 0, wrong = 0, accuracy = 0;
    char type;
    for (int i = 1; i <= 20; i++) {
        char target = rand() % 52; // 0~51 중 하나의 숫자
        if (target <= 25) target += 'a';
        else target += 'A' - 26;

        while (1) {
            accuracy = calculateAccuracy(correct, correct+wrong);
            updateLetterTexts(progress, wrong, accuracy, target);

            type = getch();
            if (type == 27) { // esc
                system("clear");
                return;
            } else if (type == target) { // 맞췄을 때
                correct++;
                break;
            } else wrong++; // 틀렸을 때
        }
        progress += 5;
    }

    // 통계 출력
    system("clear");
    printf(">> 영문 타자 연습 프로그램 : 자리 연습 <<\n");
    printf("진행도 : %d%%\t오타수 : %d\t정확도 : %d%%\n\n", progress, wrong, accuracy);
    sleep(2);
    printf("ENTER키를 누르면 메뉴로 돌아갑니다: ");
    getchar();
    return;
}

void wordTyping() {
    char wordsArr[WORDS_COUNT][20] = {
        "apple", "banana", "cherry", "date", "elderberry", "fig", "grape", "honeydew", "kiwi", "lemon",
        "mango", "nectarine", "orange", "papaya", "quince", "raspberry", "strawberry", "tangerine", "ugli", "vanilla",
        "watermelon", "xigua", "yam", "zucchini", "apricot", "blueberry", "cranberry", "dragonfruit", "eggplant", "fennel",
        "grapefruit", "huckleberry", "indigo", "jackfruit", "kumquat", "lime", "mulberry", "nectar", "olive", "pineapple",
        "quandong", "rhubarb", "spinach", "tomato", "violet", "wolfberry", "yellowfruit", "avocado", "bilberry", "coconut",
        "durian", "elder", "feijoa", "gooseberry", "honeyberry", "imbe", "jambul", "kiwifruit", "lemonade", "mangosteen",
        "nance", "oregano", "pear", "rambutan", "sapote", "tamarind", "vanilla", "waxapple", "yumberry", "acai",
        "blackberry", "clementine", "dewberry", "fig", "grapefruit", "hawthorn", "jaboticaba", "melon", "nutmeg", "pomegranate",
        "starfruit", "tangelo", "vanilla", "watermelon", "ximenia", "yellow", "zucchini", "blue", "violet", "purple",
        "orange", "white", "black", "red", "green", "brown", "gray", "elephant", "rabbit", "lion"};

    int correct = 0, wrong = 0, progress, accuracy;
    char word[20], input[100];

    for (int i = 0; i < 20; i++) {
        strcpy(word, wordsArr[rand() % WORDS_COUNT]); // word에 무작위 단어 하나 복사하기
        progress = (double)i / WORDS_LOOP * 100; // 진행도 계산
        accuracy = calculateAccuracy(correct, correct+wrong); // 정확도 계산

        updateWordTexts(progress, wrong, accuracy, word);
        scanf("%s", input);

        if (strcmp(input, "###") == 0) return; // ### 입력 시 메뉴로 나가기
        else if (strcmp(word, input) == 0) correct++; // 단어가 같을 때
        else wrong++; // 다를 때

        getchar();  // scanf 하고 난 뒤의 개행 문자를 읽기 위해
    }

    progress = 100;
    accuracy = calculateAccuracy(correct, correct+wrong);

    // 통계 출력
    system("clear");
    printf(">> 영문 타자 연습 프로그램 : 낱말 연습 <<\n");
    printf("진행도 : %d%%\t오타수 : %d\t정확도 : %d%%\n\n", progress, wrong, accuracy);

    sleep(2);
    printf("연습이 끝났습니다! 엔터 키를 누르면 메뉴로 돌아갑니다.\n");
    getchar();
    return;
}

void shortTyping() {
    char shortsArr[SHORTS_COUNT][100] = {
        "The quick brown fox jumps over the lazy dog.",
        "Every moment is a fresh beginning.",
        "Change the world by being yourself.",
        "Never regret anything that made you smile.",
        "Die with memories, not dreams.",
        "Aspire to inspire before we expire.",
        "Everything you can imagine is real.",
        "Simplicity is the ultimate sophistication.",
        "Whatever you do, do it well.",
        "What we think, we become.",
        "All limitations are self-imposed.",
        "Tough times never last but tough people do.",
        "Problems are not stop signs, they are guidelines.",
        "One day the people that don't even believe in you will tell everyone how they met you.",
        "If I'm gonna tell a real story, I'm gonna start with my name.",
        "If you tell the truth you don't have to remember anything.",
        "Have enough courage to start and enough heart to finish.",
        "Hate comes from intimidation, love comes from appreciation.",
        "I could agree with you but then we'd both be wrong.",
        "Oh, the things you can find, if you don't stay behind.",
        "Determine your priorities and focus on them.",
        "Be so good they can't ignore you.",
        "Dream as if you'll live forever, live as if you'll die today.",
        "Yesterday you said tomorrow. Just do it.",
        "I don't need it to be easy, I need it to be worth it.",
        "Never let your emotions overpower your intelligence.",
        "Nothing lasts forever but at least we got these memories.",
        "Don't you know your imperfections is a blessing?",
        "Reality is wrong, dreams are for real.",
        "To live will be an awfully big adventure."};

    char c, target[100], input[100];
    int progress = 0, cpm, maxCpm = 0, correct, total, accuracy;
    int start;

    for (int i = 1; i <= SHORTS_LOOP; i++) {
        strcpy(target, shortsArr[rand() % SHORTS_COUNT]); // target 변수에 무작위 문장 저장
        input[0] = '\0'; // 사용자 입력을 저장하는 문자열 변수를 초기화
        correct = total = accuracy = cpm = 0;  // 새로운 문장이 시작되면 정확도와 현재타수는 다시 계산된다.
        start = time(NULL); // 시작 시간

        for (int p = 0; ;) {
            updateShortTexts(progress, cpm, maxCpm, accuracy, target, input); // 상단 텍스트 업데이트
            c = getch();
            if (c == '~') return;   // 5번의 문장을 입력하기 전에 메뉴로 복귀할 수 있는 기능
            else if (c == 10) {        // 엔터
                if (p < 1) continue;  // 문장을 입력한 후 눌렀을 경우만 실행.
                // 엔터키를 누르면 문장을 처리 한다.
                while (target[p++] != '\0') total++;
                if (correct < 0) correct = 0;
                accuracy = calculateAccuracy(correct, total);
                break; // for문 나가기
            } else if (c == 8 || c == 127) {  // 백스페이스
                if (p < 1) continue; // 입력한 문자 없을 때 건너뛰기
                p--, total--;
                if (input[p] == target[p]) correct--; // 전에 입력했던 게 맞았었을 경우
                input[p] = '\0';  // 문자열 끝 표기하기
            } else {
                input[p] = c, input[p+1] = '\0';
                if (c == target[p]) correct++; // 입력한게 타겟 문장의 글자와 일치할 때
                total++, p++;
            }
            accuracy = calculateAccuracy(correct, total);
            cpm = calculateCpm(correct, start);
            if (maxCpm < cpm) maxCpm = cpm; // 최고타수 기록
        }
        progress = (double)i / SHORTS_LOOP * 100;
    }

    // 통계 출력
    system("clear");
    printf(">> 짧은 글 연습 결과 <<\n");
    printf("진행도 : %d%%\t최고타수 : %d\n\n", progress, maxCpm);

    sleep(2);
    printf("메뉴로 나가려면 q 입력: ");
    while (getch() != 'q');
    return;
}

void longTyping() {
    char longsArr[4][10][100] = {
        {"The Selfish Giant",
         "Every afternoon, as they were coming from school, the children used",
         "to go and play in the Giant’s garden.",
         "It was a large lovely garden, with soft green grass. Here and there",
         "over the grass stood beautiful flowers like stars, and there were",
         "twelve peach trees that in the spring-time broke out into delicate blossoms",
         "of pink and pearl, and in the autumn bore rich fruit.",
         "The birds sat on the trees and sang so sweetly that the children used",
         "to stop their games in order to listen to them.",
         "\"How happy we are here!\" they cried to each other."},
        {"To be, or not to be, that is the question:",
         "Whether 'tis nobler in the mind to suffer",
         "The slings and arrows of outrageous fortune,",
         "Or to take arms against a sea of troubles",
         "And by opposing end them. To die: to sleep;",
         "No more; and by a sleep to say we end",
         "The heart-ache and the thousand natural shocks",
         "That flesh is heir to, 'tis a consummation",
         "Devoutly to be wish'd. To die, to sleep;",
         "To sleep: perchance to dream: ay, there's the rub;"},
        {"It was the best of times, it was the worst of times,",
         "it was the age of wisdom, it was the age of foolishness,",
         "it was the epoch of belief, it was the epoch of incredulity,",
         "it was the season of Light, it was the season of Darkness,",
         "it was the spring of hope, it was the winter of despair,",
         "we had everything before us, we had nothing before us,",
         "we were all going direct to Heaven, we were all going direct the other way –",
         "in short, the period was so far like the present period, that some of its",
         "noisiest authorities insisted on its being received, for good or for evil,",
         "in the superlative degree of comparison only."},
        {"All happy families are alike;",
         "each unhappy family is unhappy in its own way.",
         "Everything was in confusion in the Oblonskys' house.",
         "The wife had discovered that the husband was carrying on an intrigue",
         "with a French girl, who had been a governess in their family.",
         "She had announced that she could not go on living in the same house with him.",
         "This position of affairs had now lasted three days, and not only the",
         "husband and wife themselves, but all the members of their family and",
         "household, were painfully conscious of it. Every person in the house",
         "felt that there was no sense in their living together, and that the stray"}};

    char c, input[5][100];
    int target, line = 0;
    int cpm = 0, correct = 0, total = 0, accuracy = 0;
    int start;

    input[0][0] = '\0';
    target = rand() % 4; // 0~3 의 무작위 숫자로 목표 글을 정하기
    /* int longsLength[4], sum;
    for (int i = 0; i < 4; i++) {
        sum = 0;
        for (int j = 0; j < 10; j++)
            sum += strlen(longsArr[i][j]);
        longsLength[i] = sum;
    } */
    start = time(NULL); // 시작 시간

    for (int p = 0; ;) {
        if (line >= 10) break; // 10개의 행 모두 입력해서 끝났을 때
       
        updateLongTexts(accuracy, cpm, line, longsArr[target], input);
        c = getch();
        if (c == '~') return;       // 모두 입력하기 전에 메뉴로 복귀할 수 있는 기능
        else if (c == 10) {        // 엔터
            // 문장에 남은 글자가 있는데 엔터를 눌렀을 때 남은 글자 처리
            while (longsArr[target][line][p++] != '\0') total++;
            if (correct < 0) correct = 0;
            accuracy = calculateAccuracy(correct, total);
            p = 0, line++;
            input[line%5][0] = '\0'; // input배열의 다음 line 행 초기화
            continue;
        } else if (c == 8 || c == 127) {  // 백스페이스
            if (p <= 0) continue; // 입력한 글자 없으면 건너뛰기
            p--, total--;
            if (input[line%5][p] == longsArr[target][line][p]) correct--; // 지울 글자가 목표 글자와 일치했을 때
            input[line%5][p] = '\0';  // 문자열 끝 표기하기
        } else {
            input[line%5][p] = c, input[line%5][p+1] = '\0';
            if (c == longsArr[target][line][p]) correct++;
            total++, p++;
        }
        accuracy = calculateAccuracy(correct, total);
        cpm = calculateCpm(correct, start);
    }

    // 통계 출력
    system("clear");
    printf(">> 긴 글 연습 결과 <<\n");
    printf("정확도 : %d%%\t걸린 시간 : %ld초\n\n", accuracy, time(NULL)-start);

    sleep(2);
    printf("메뉴로 나가려면 q 입력: ");
    while (getch() != 'q');
    return;
}

int main() {
    srand(time(NULL));

    int mode;
    while (1) {
        system("clear");
        printf(">> 영문 타자 연습 프로그램 <<\n1. 자리 연습\t2. 낱말 연습\n3. 짧은 글 연습\t 4. 긴 글 연습\n5. 프로그램 종료\n\n");
        printf("번호를 선택하세요: ");
        scanf("%d", &mode);
        getchar();  // 개행문자 비우기
        switch (mode) {
            case 1:
                letterTyping();
                break;
            case 2:
                wordTyping();
                break;
            case 3:
                shortTyping();
                break;
            case 4:
                longTyping();
                break;
            case 5:
                system("clear");
                return 0;
        }
    }
    return 0;
}