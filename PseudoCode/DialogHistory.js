class Owner {
	constructor(Name) {
		this.Name = Name;
	}
}
class DialogueBlock {
	constructor(
		Name,
		NormalizedPoint,
		RealEarnedPoint,
		PointType,
		SentencePosition
	) {
		this.Name = Name;
		this.NormalizedPoint = NormalizedPoint;
		this.RealEarnedPoint = RealEarnedPoint;
		this.PointType = PointType;
		this.SentencePosition = SentencePosition;
	}
}

class DialogueSequence {
	constructor(Name, DialogueBlocks, Owner, id) {
		this.Name = Name;
		this.DialogueBlocks = DialogueBlocks;
		this.DialogueOwner = Owner;
		this.Id = id;
	}
}

class DialogueHistory {
	constructor(LevelName, LevelId, Sequences) {
		this.LevelName = LevelName;
		this.LevelId = LevelId;
		this.Sequences = Sequences;
	}
}

class FPointType {
	constructor(Name) {
		this.Name = Name;
	}
}

const AvailablePointTypes = [
	new FPointType("CNV"),
	new FPointType("CSV"),
	new FPointType("Diplomacy"),
	new FPointType("Manipulation")
];

const Marcel = new Owner("Marcel");
const Jack = new Owner("Jack");

const Sequence1Marcel = new DialogueSequence(
	"Sequence1",
	[
		new DialogueBlock("Block1", 1, 1.5, AvailablePointTypes[0], 0),
		new DialogueBlock("Block2", 2, 4, AvailablePointTypes[1], 1)
	],
	Marcel,
	1
);
const Sequence1MarcelBis = new DialogueSequence(
	"Sequence1",
	[
		new DialogueBlock("Block1", 1, 3, AvailablePointTypes[0], 0),
		new DialogueBlock("Block2", 2, 6, AvailablePointTypes[1], 1)
	],
	Marcel,
	2
);
const Sequence2Marcel = new DialogueSequence(
	"Sequence2",
	[
		new DialogueBlock("Block3", 3, 2, AvailablePointTypes[2], 2),
		new DialogueBlock("Block4", 1, 3, AvailablePointTypes[3], 2)
	],
	Marcel,
	3
);
const Sequence1Jack = new DialogueSequence(
	"Sequence1",
	[
		new DialogueBlock("Block1", 1, 3, AvailablePointTypes[0], 0),
		new DialogueBlock("Block2", 3, 1, AvailablePointTypes[3], 2)
	],
	Jack,
	4
);
const Sequence3Jack = new DialogueSequence(
	"Sequence3",
	[
		new DialogueBlock("Block5", 1, 3, AvailablePointTypes[3], 4),
		new DialogueBlock("Block6", 3, 1, AvailablePointTypes[2], 5)
	],
	Jack,
	5
);

const DialogueHistories = [
	new DialogueHistory("Level1", 1, [Sequence1Marcel, Sequence1Jack]),
	new DialogueHistory("Level2", 3, [Sequence2Marcel, Sequence3Jack]),
	new DialogueHistory("Level1", 2, [Sequence1MarcelBis, Sequence1Jack])
];

const currentLevelId = 2;
const currentSequence = Sequence3Jack;

class Search {
	constructor(DialogueHistories) {
		this.DialogueHistories = DialogueHistories;
	}

	searchSequenceById(Id) {}

	SearchBlocks(search) {
		let Results = [];
		let searchChunks = search.split(".");
		for (let history of DialogueHistories) {
			if (
				searchChunks[0] != "*" &&
				searchChunks[0] != history.LevelName
			) {
				continue;
			}
			if (
				searchChunks[0] == "None" &&
				history.LevelId != currentLevelId
			) {
				continue;
			}

			for (let sequence of history.Sequences) {
				if (
					searchChunks[1] != "*" &&
					searchChunks[1] != "None" &&
					searchChunks[1] != sequence.Name
				) {
					continue;
				}
				if (
					searchChunks[1] == "None" &&
					sequence.Id != currentSequence.Id
				) {
					continue;
				}
				if (
					searchChunks[2] != "*" &&
					searchChunks[2] != sequence.DialogueOwner.Name
				) {
					continue;
				}
				if (
					searchChunks[2] == "None" &&
					sequence.DialogueOwner != currentSequence.DialogueOwner
				) {
					continue;
				}

				for (let block of sequence.DialogueBlocks) {
					if (
						searchChunks[3] != "*" &&
						searchChunks[3] != block.Name
					) {
						continue;
					}
					Results.push(block);
				}
			}
		}
		return Results;
	}
}

const searches = [
	"Level1.Sequence1.Marcel.*.PointType",
	"Level1.*.*.*.PointType",
	"Level1.*.Marcel.*.PointType",
	"Level1.*.Marcel.Block1.PointType",
	"*.*.Marcel.*.PointType",
	"*.*.*.*.PointType"
];
const Seeker = new Search(DialogueHistories);

for (search of searches) {
	let BlocksResults = Seeker.SearchBlocks(search);
	console.log(
		`\n\n============================================================`
	);
	console.log(`Values for search ${search}:`);
	console.log(`============================================================`);
	console.log(BlocksResults);
}
