/*
  * Copyright (C) 2011 Cameron White
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
  
#include "scorearea.h"

#include <app/documentmanager.h>
#include <app/pubsub/scorelocationpubsub.h>
#include <app/pubsub/staffpubsub.h>
#include <chrono>
#include <painters/caretpainter.h>
#include <painters/systemrenderer.h>
#include <QDebug>
#include <QGraphicsItem>
#include <score/score.h>
#include <thread>

static const double SYSTEM_SPACING = 50;

ScoreArea::ScoreArea(QWidget *parent)
    : QGraphicsView(parent),
      myViewType(Staff::GuitarView),
      myCaretPainter(nullptr),
      myKeySignatureClicked(std::make_shared<ScoreLocationPubSub>()),
      myTimeSignatureClicked(std::make_shared<ScoreLocationPubSub>()),
      myBarlineClicked(std::make_shared<ScoreLocationPubSub>()),
      myClefClicked(std::make_shared<StaffPubSub>())
{
    setScene(&myScene);
}

void ScoreArea::renderDocument(const Document &document, Staff::ViewType view)
{
    myScene.clear();
    myRenderedSystems.clear();
    myDocument = document;
    myViewType = view;

    const Score &score = document.getScore();

    auto start = std::chrono::high_resolution_clock::now();

    myCaretPainter = new CaretPainter(document.getCaret());
    myCaretPainter->subscribeToMovement([=]() {
        adjustScroll();
    });

    myRenderedSystems.reserve(score.getSystems().size());
    for (const System &system : score.getSystems())
        myRenderedSystems.append(nullptr);

#if 0
    const int numThreads = std::thread::hardware_concurrency();
#else
    const int numThreads = 1;
#endif

    std::vector<std::thread> workers;
    const int workSize = myRenderedSystems.size() / numThreads;
    qDebug() << "Using " << numThreads << " worker thread(s)";

    for (int i = 0; i < numThreads; ++i)
    {
        const int left = i * workSize;
        const int right = (i == numThreads - 1) ? myRenderedSystems.size()
                                                : (i + 1) * workSize;

        workers.emplace_back([&](int left, int right)
        {
            for (int i = left; i < right; ++i)
            {
                SystemRenderer render(this, score);
                myRenderedSystems[i] = render(score.getSystems()[i], i, myViewType);
            }
        }, left, right);
    }

    for (std::thread &worker : workers)
        worker.join();

    int i = 0;
    double height = 0;
    // Layout the systems.
    for (QGraphicsItem *system : myRenderedSystems)
    {
        system->setPos(0, height);
        myScene.addItem(system);
        height += system->boundingRect().height() + SYSTEM_SPACING;
        ++i;

        myCaretPainter->addSystemRect(system->sceneBoundingRect());
    }

    myScene.addItem(myCaretPainter);

    auto end = std::chrono::high_resolution_clock::now();
    qDebug() << "Score rendered in"
             << std::chrono::duration_cast<std::chrono::milliseconds>(
                    end - start).count() << "ms";
    qDebug() << "Rendered " << myScene.items().size() << "items";
}

void ScoreArea::redrawSystem(int index)
{
    // Delete and remove the system from the scene.
    delete myRenderedSystems.takeAt(index);

    const Score &score = myDocument->getScore();
    SystemRenderer render(this, score);
    QGraphicsItem *newSystem = render(score.getSystems()[index], index, myViewType);

    double height = 0;
    if (index > 0)
    {
        height = myRenderedSystems.at(index - 1)->sceneBoundingRect().bottom() +
                SYSTEM_SPACING;
    }

    newSystem->setPos(0, height);
    height += newSystem->boundingRect().height() + SYSTEM_SPACING;
    myCaretPainter->setSystemRect(index, newSystem->sceneBoundingRect());

    myScene.addItem(newSystem);
    myRenderedSystems.insert(index, newSystem);

    // Shift the following systems.
    for (int i = index + 1; i < myRenderedSystems.size(); ++i)
    {
        QGraphicsItem *system = myRenderedSystems[i];
        system->setPos(0, height);
        height += system->boundingRect().height() + SYSTEM_SPACING;
        myCaretPainter->setSystemRect(i, system->sceneBoundingRect());
    }
}

std::shared_ptr<ScoreLocationPubSub> ScoreArea::getKeySignaturePubSub() const
{
    return myKeySignatureClicked;
}

std::shared_ptr<ScoreLocationPubSub> ScoreArea::getTimeSignaturePubSub() const
{
    return myTimeSignatureClicked;
}

std::shared_ptr<ScoreLocationPubSub> ScoreArea::getBarlinePubSub() const
{
    return myBarlineClicked;
}

std::shared_ptr<ScoreLocationPubSub> ScoreArea::getSelectionPubSub() const
{
    return myDocument->getCaret().getSelectionPubSub();
}

std::shared_ptr<StaffPubSub> ScoreArea::getClefPubSub() const
{
    return myClefClicked;
}

void ScoreArea::adjustScroll()
{
    ensureVisible(myCaretPainter->sceneBoundingRect(), 0, 100);
}
